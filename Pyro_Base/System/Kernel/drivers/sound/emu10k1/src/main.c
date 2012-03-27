 /*
 **********************************************************************
 *     main.c - Creative EMU10K1 audio driver
 *     Copyright 1999, 2000 Creative Labs, Inc.
 *
 **********************************************************************
 *
 *     Date                 Author          Summary of changes
 *     ----                 ------          ------------------
 *     October 20, 1999     Bertrand Lee    base code release
 *     November 2, 1999     Alan Cox        cleaned up stuff
 *
 **********************************************************************
 *
 *     This program is free software; you can redistribute it and/or
 *     modify it under the terms of the GNU General Public License as
 *     published by the Free Software Foundation; either version 2 of
 *     the License, or (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public
 *     License along with this program; if not, write to the Free
 *     Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139,
 *     USA.
 *
 **********************************************************************
 *
 *      Supported devices:
 *      /dev/dsp:        Standard /dev/dsp device, OSS-compatible
 *      /dev/dsp1:       Routes to rear speakers only	 
 *      /dev/mixer:      Standard /dev/mixer device, OSS-compatible
 *      /dev/midi:       Raw MIDI UART device, mostly OSS-compatible
 *	/dev/sequencer:  Sequencer Interface (requires sound.o)
 *
 *      Revision history:
 *      0.1 beta Initial release
 *      0.2 Lowered initial mixer vol. Improved on stuttering wave playback. Added MIDI UART support.
 *      0.3 Fixed mixer routing bug, added APS, joystick support.
 *      0.4 Added rear-channel, SPDIF support.
 *	0.5 Source cleanup, SMP fixes, multiopen support, 64 bit arch fixes,
 *	    moved bh's to tasklets, moved to the new PCI driver initialization style.
 *	0.6 Make use of pci_alloc_consistent, improve compatibility layer for 2.2 kernels,
 *	    code reorganization and cleanup.
 *	0.7 Support for the Emu-APS. Bug fixes for voice cache setup, mmaped sound + poll().
 *          Support for setting external TRAM size.
 *      0.8 Make use of the kernel ac97 interface. Support for a dsp patch manager.
 *      0.9 Re-enables rear speakers volume controls
 *     0.10 Initializes rear speaker volume.
 *	    Dynamic patch storage allocation.
 *	    New private ioctls to change control gpr values.
 *	    Enable volume control interrupts.
 *	    By default enable dsp routes to digital out. 
 *     0.11 Fixed fx / 4 problem.
 *     0.12 Implemented mmaped for recording.
 *	    Fixed bug: not unreserving mmaped buffer pages.
 *	    IRQ handler cleanup.
 *     0.13 Fixed problem with dsp1
 *          Simplified dsp patch writing (inside the driver)
 *	    Fixed several bugs found by the Stanford tools
 *     0.14 New control gpr to oss mixer mapping feature (Chris Purnell)
 *          Added AC3 Passthrough Support (Juha Yrjola)
 *          Added Support for 5.1 cards (digital out and the third analog out)
 *     0.15 Added Sequencer Support (Daniel Mack)
 *          Support for multichannel pcm playback (Eduard Hasenleithner)
 *     0.16 Mixer improvements, added old treble/bass support (Daniel Bertrand)
 *          Small code format cleanup.
 *          Deadlock bug fix for emu10k1_volxxx_irqhandler().
 *     0.17 Fix for mixer SOUND_MIXER_INFO ioctl.
 *	    Fix for HIGHMEM machines (emu10k1 can only do 31 bit bus master) 
 *	    midi poll initial implementation.
 *	    Small mixer fixes/cleanups.
 *	    Improved support for 5.1 cards.
 *     0.18 Fix for possible leak in pci_alloc_consistent()
 *          Cleaned up poll() functions (audio and midi). Don't start input.
 *	    Restrict DMA pages used to 512Mib range.
 *	    New AC97_BOOST mixer ioctl.
 *    0.19a Added Support for Audigy Cards
 *	    Real fix for kernel with highmem support (cast dma_handle to u32).
 *	    Fix recording buffering parameters calculation.
 *	    Use unsigned long for variables in bit ops.
 *    0.20a Fixed recording startup
 *	    Fixed timer rate setting (it's a 16-bit register)
 *********************************************************************/

/* These are only included once per module */
#include <pyro/kernel.h>
#include <pyro/pci.h>
#include <pyro/device.h>
#include <pyro/irq.h>
#include <pyro/bitops.h>
#include <posix/errno.h>
#include <posix/signal.h>

#include "hwaccess.h"
#include "8010.h"
#include "efxmgr.h"
#include "cardwo.h"

/* this should be in dev_table.h */
#define SNDCARD_EMU10K1 46
 
/* the emu10k1 should supprt 31 bit bus master */
#define EMU10K1_DMA_MASK                0x7fffffff	/* DMA buffer mask for pci_alloc_consist */

#ifndef PCI_VENDOR_ID_CREATIVE
#define PCI_VENDOR_ID_CREATIVE 0x1102
#endif

#ifndef PCI_DEVICE_ID_CREATIVE_EMU10K1
#define PCI_DEVICE_ID_CREATIVE_EMU10K1 0x0002
#endif

#ifndef PCI_DEVICE_ID_CREATIVE_AUDIGY
#define PCI_DEVICE_ID_CREATIVE_AUDIGY 0x0004
#endif

#define EMU_APS_SUBID	0x40011102
 
enum {
	EMU10K1 = 0,
	AUDIGY,
};

static char *card_names[] = {
	"SBLive!",
	"Audigy",
};

/* Global var instantiation */

LIST_HEAD(emu10k1_devs);

extern DeviceOperations_s emu10k1_audio_fops;
extern DeviceOperations_s emu10k1_mixer_fops;

struct emu10k1_card* sblive_card;

extern int emu10k1_interrupt(int irq, void *pdata, SysCallRegs_s* regs);

static int emu10k1_audio_init(struct emu10k1_card *card, int nHandle)
{
	card->audio_dev = create_device_node(card->dev_id, nHandle, "audio/emu10k1", &emu10k1_audio_fops, card);
	if (card->audio_dev < 0)
	{
		printk("emu10k1: cannot register first audio device!\n");
		goto err_dev;
	}

	/* Assign default playback voice parameters */
	if (card->is_audigy)
		card->mchannel_fx = 0;
	else
		card->mchannel_fx = 8;

	if (card->is_audigy) {
		/* mono voice */
		card->waveout.send_dcba[SEND_MONO] = 0xffffffff;
		card->waveout.send_hgfe[SEND_MONO] = 0x0000ffff;
	
		/* stereo voice */
		/* left */
		card->waveout.send_dcba[SEND_LEFT] = 0x00ff00ff;
		card->waveout.send_hgfe[SEND_LEFT] = 0x00007f7f;	
		/* right */
		card->waveout.send_dcba[SEND_RIGHT] = 0xff00ff00;
		card->waveout.send_hgfe[SEND_RIGHT] = 0x00007f7f;

		card->waveout.send_routing[ROUTE_PCM] = 0x03020100; // Regular pcm
		card->waveout.send_routing2[ROUTE_PCM] = 0x07060504;

		card->waveout.send_routing[ROUTE_PT] = 0x3f3f3d3c; // Passthrough
		card->waveout.send_routing2[ROUTE_PT] = 0x3f3f3f3f;
		
		card->waveout.send_routing[ROUTE_PCM1] = 0x03020100; // Spare
		card->waveout.send_routing2[ROUTE_PCM1] = 0x07060404;
		
	} else {
		/* mono voice */
		card->waveout.send_dcba[SEND_MONO] = 0x0000ffff;
	
		/* stereo voice */
		/* left */
		card->waveout.send_dcba[SEND_LEFT] = 0x000000ff;
		/* right */
		card->waveout.send_dcba[SEND_RIGHT] = 0x0000ff00;

		card->waveout.send_routing[ROUTE_PCM] = 0x3210; // pcm
		card->waveout.send_routing[ROUTE_PT] = 0x3210; // passthrough
		card->waveout.send_routing[ROUTE_PCM1] = 0x7654; // /dev/dsp1
	}

	return 0;

err_dev:
	return -ENODEV;
}

static void emu10k1_audio_cleanup(struct emu10k1_card *card)
{
	delete_device_node(card->audio_dev);
}

static int emu10k1_mixer_init(struct emu10k1_card *card, int nHandle)
{
	struct ac97_codec *codec = &card->ac97;
	card->ac97.dev_mixer = create_device_node(card->dev_id, nHandle, "audio/mixer/emu10k1", &emu10k1_mixer_fops, card);

	if (card->ac97.dev_mixer < 0)
	{
		printk("emu10k1: cannot register mixer device\n");
		return -EIO;
	}

	card->ac97.private_data = card;

	if (!card->is_aps)
	{
		card->ac97.id = 0;
		card->ac97.codec_read = emu10k1_ac97_read;
        	card->ac97.codec_write = emu10k1_ac97_write;

		if (ac97_probe_codec (&card->ac97) == 0)
		{
			printk("emu10k1: unable to probe AC97 codec\n");
			goto err_out;
		}
		/* 5.1: Enable the additional AC97 Slots and unmute extra channels on AC97 codec */

		if (codec->codec_read(codec, AC97_EXTENDED_ID) & 0x0080)
		{
			printk("emu10k1: SBLive! 5.1 card detected\n"); 
			sblive_writeptr(card, AC97SLOT, 0, AC97SLOT_CNTR | AC97SLOT_LFE);
			codec->codec_write(codec, AC97_SURROUND_MASTER, 0x0);
		}

		/* these will store the original values and never be modified */
		card->ac97_supported_mixers = card->ac97.supported_mixers;
		card->ac97_stereo_mixers = card->ac97.stereo_mixers;
	}

	return 0;

err_out:
	delete_device_node(card->ac97.dev_mixer);
	return -EIO;
}

static void emu10k1_mixer_cleanup(struct emu10k1_card *card)
{
	delete_device_node(card->ac97.dev_mixer);
}

static void voice_init(struct emu10k1_card *card)
{
	int i;

	for (i = 0; i < NUM_G; i++)
		card->voicetable[i] = VOICE_USAGE_FREE;
}

static void timer_init(struct emu10k1_card *card)
{
	INIT_LIST_HEAD(&card->timers);
	card->timer_delay = TIMER_STOPPED;
	spinlock_init(&card->timer_lock,"emu10k1_timer_lock");
}

static void addxmgr_init(struct emu10k1_card *card)
{
	uint32 count;

	for (count = 0; count < MAXPAGES; count++)
		card->emupagetable[count] = 0;

	/* Mark first page as used */
	/* This page is reserved by the driver */
	card->emupagetable[0] = 0x8001;
	card->emupagetable[1] = MAXPAGES - 1;

	emu10k1_addxmgr_alloc(5*PAGE_SIZE, card);
}

static void fx_cleanup(struct patch_manager *mgr)
{
	int i;

	for(i = 0; i < mgr->current_pages; i++)
		kfree(mgr->patch[i]);
}

static int fx_init(struct emu10k1_card *card)
{
	struct patch_manager *mgr = &card->mgr;
	struct dsp_patch *patch;
	struct dsp_rpatch *rpatch;
	int32 left, right;
	int i;
	uint32 pc = 0;
	uint32 patch_n;
	struct emu_efx_info_t emu_efx_info[2]=
		{{ 20, 10, 0x400, 0x100, 0x20 },
		 { 24, 12, 0x600, 0x400, 0x60 },
		}; 


	for (i = 0; i < SOUND_MIXER_NRDEVICES; i++) {
		mgr->ctrl_gpr[i][0] = -1;
		mgr->ctrl_gpr[i][1] = -1;
	}

	for (i = 0; i < 512; i++)
		OP(6, 0x40, 0x40, 0x40, 0x40);

	for (i = 0; i < 256; i++)
		sblive_writeptr_tag(card, 0,
				    FXGPREGBASE + i, 0,
				    TANKMEMADDRREGBASE + i, 0,
				    TAGLIST_END);

	if (card->is_audigy)
		mgr->current_pages = (2 + PATCHES_PER_PAGE - 1) / PATCHES_PER_PAGE;
	else
		/* !! The number below must equal the number of patches, currently 11 !! */
		mgr->current_pages = (11 + PATCHES_PER_PAGE - 1) / PATCHES_PER_PAGE;

	for (i = 0; i < mgr->current_pages; i++) {
		mgr->patch[i] = kmalloc(4096,MEMF_KERNEL);
		if (mgr->patch[i] == NULL) {
			mgr->current_pages = i;
			fx_cleanup(mgr);
			return -ENOMEM;
		}
		memset(mgr->patch[i], 0, PAGE_SIZE);
	}

	pc = 0;
	patch_n = 0;
	//first free GPR = 0x11b

	if (card->is_audigy) {
		for (i = 0; i < 1024; i++)
			OP(0xf, 0x0c0, 0x0c0, 0x0cf, 0x0c0);

		for (i = 0; i < 512 ; i++)
			sblive_writeptr(card, A_GPR_BASE+i,0,0);

		pc=0;

		//Pcm input volume
		OP(0, 0x402, 0x0c0, 0x406, 0x000);
		OP(0, 0x403, 0x0c0, 0x407, 0x001);

		//CD-Digital input Volume
		OP(0, 0x404, 0x0c0, 0x40d, 0x42);
		OP(0, 0x405, 0x0c0, 0x40f, 0x43);

		// CD + PCM 
		OP(6, 0x400, 0x0c0, 0x402, 0x404);
		OP(6, 0x401, 0x0c0, 0x403, 0x405);
		
		// Front Output + Master Volume
		OP(0, 0x68, 0x0c0, 0x408, 0x400);
		OP(0, 0x69, 0x0c0, 0x409, 0x401);

		// Add-in analog inputs for other speakers
		OP(6, 0x400, 0x40, 0x400, 0xc0);
		OP(6, 0x401, 0x41, 0x401, 0xc0);

		// Digital Front + Master Volume
		OP(0, 0x60, 0x0c0, 0x408, 0x400);
		OP(0, 0x61, 0x0c0, 0x409, 0x401);

		// Rear Output + Rear Volume
		OP(0, 0x06e, 0x0c0, 0x419, 0x400);
		OP(0, 0x06f, 0x0c0, 0x41a, 0x401);		

		// Digital Rear Output + Rear Volume
		OP(0, 0x066, 0x0c0, 0x419, 0x400);
		OP(0, 0x067, 0x0c0, 0x41a, 0x401);		

		// Audigy Drive, Headphone out
		OP(6, 0x64, 0x0c0, 0x0c0, 0x400);
		OP(6, 0x65, 0x0c0, 0x0c0, 0x401);

		// ac97 Recording
		OP(6, 0x76, 0x0c0, 0x0c0, 0x40);
		OP(6, 0x77, 0x0c0, 0x0c0, 0x41);
		
		// Center = sub = Left/2 + Right/2
		OP(0xe, 0x400, 0x401, 0xcd, 0x400);

		// center/sub  Volume (master)
		OP(0, 0x06a, 0x0c0, 0x408, 0x400);
		OP(0, 0x06b, 0x0c0, 0x409, 0x400);

		// Digital center/sub  Volume (master)
		OP(0, 0x062, 0x0c0, 0x408, 0x400);
		OP(0, 0x063, 0x0c0, 0x409, 0x400);

		ROUTING_PATCH_START(rpatch, "Routing");
		ROUTING_PATCH_END(rpatch);

		/* delimiter patch */
		patch = PATCH(mgr, patch_n);
		patch->code_size = 0;

	
		sblive_writeptr(card, 0x53, 0, 0);
	} else {
		for (i = 0; i < 512 ; i++)
			OP(6, 0x40, 0x40, 0x40, 0x40);

		for (i = 0; i < 256; i++)
			sblive_writeptr_tag(card, 0,
					    FXGPREGBASE + i, 0,
					    TANKMEMADDRREGBASE + i, 0,
					    TAGLIST_END);

		
		pc = 0;

		//first free GPR = 0x11b
	
		
		/* FX volume correction and Volume control*/
		INPUT_PATCH_START(patch, "Pcm L vol", 0x0, 0);
		GET_OUTPUT_GPR(patch, 0x100, 0x0);
		GET_CONTROL_GPR(patch, 0x106, "Vol", 0, 0x7fffffff);
		GET_DYNAMIC_GPR(patch, 0x112);

		OP(4, 0x112, 0x40, PCM_IN_L, 0x44); //*4	
		OP(0, 0x100, 0x040, 0x112, 0x106);  //*vol	
		INPUT_PATCH_END(patch);


		INPUT_PATCH_START(patch, "Pcm R vol", 0x1, 0);
		GET_OUTPUT_GPR(patch, 0x101, 0x1);
		GET_CONTROL_GPR(patch, 0x107, "Vol", 0, 0x7fffffff);
		GET_DYNAMIC_GPR(patch, 0x112);

		OP(4, 0x112, 0x40, PCM_IN_R, 0x44); 
		OP(0, 0x101, 0x040, 0x112, 0x107);

		INPUT_PATCH_END(patch);


		// CD-Digital In Volume control	
		INPUT_PATCH_START(patch, "CD-Digital Vol L", 0x12, 0);
		GET_OUTPUT_GPR(patch, 0x10c, 0x12);
		GET_CONTROL_GPR(patch, 0x10d, "Vol", 0, 0x7fffffff);

		OP(0, 0x10c, 0x040, SPDIF_CD_L, 0x10d);
		INPUT_PATCH_END(patch);

		INPUT_PATCH_START(patch, "CD-Digital Vol R", 0x13, 0);
		GET_OUTPUT_GPR(patch, 0x10e, 0x13);
		GET_CONTROL_GPR(patch, 0x10f, "Vol", 0, 0x7fffffff);

		OP(0, 0x10e, 0x040, SPDIF_CD_R, 0x10f);
		INPUT_PATCH_END(patch);

		//Volume Correction for Multi-channel Inputs	
		INPUT_PATCH_START(patch, "Multi-Channel Gain", 0x08, 0);
		patch->input=patch->output=0x3F00;

		GET_OUTPUT_GPR(patch, 0x113, MULTI_FRONT_L);
		GET_OUTPUT_GPR(patch, 0x114, MULTI_FRONT_R);
		GET_OUTPUT_GPR(patch, 0x115, MULTI_REAR_L);
		GET_OUTPUT_GPR(patch, 0x116, MULTI_REAR_R);
		GET_OUTPUT_GPR(patch, 0x117, MULTI_CENTER);
		GET_OUTPUT_GPR(patch, 0x118, MULTI_LFE);

		OP(4, 0x113, 0x40, MULTI_FRONT_L, 0x44);
		OP(4, 0x114, 0x40, MULTI_FRONT_R, 0x44);
		OP(4, 0x115, 0x40, MULTI_REAR_L, 0x44);
		OP(4, 0x116, 0x40, MULTI_REAR_R, 0x44);
		OP(4, 0x117, 0x40, MULTI_CENTER, 0x44);
		OP(4, 0x118, 0x40, MULTI_LFE, 0x44);
	
		INPUT_PATCH_END(patch);


		//Routing patch start	
		ROUTING_PATCH_START(rpatch, "Routing");
		GET_INPUT_GPR(rpatch, 0x100, 0x0);
		GET_INPUT_GPR(rpatch, 0x101, 0x1);
		GET_INPUT_GPR(rpatch, 0x10c, 0x12);
		GET_INPUT_GPR(rpatch, 0x10e, 0x13);
		GET_INPUT_GPR(rpatch, 0x113, MULTI_FRONT_L);
		GET_INPUT_GPR(rpatch, 0x114, MULTI_FRONT_R);
		GET_INPUT_GPR(rpatch, 0x115, MULTI_REAR_L);
		GET_INPUT_GPR(rpatch, 0x116, MULTI_REAR_R);
		GET_INPUT_GPR(rpatch, 0x117, MULTI_CENTER);
		GET_INPUT_GPR(rpatch, 0x118, MULTI_LFE);

		GET_DYNAMIC_GPR(rpatch, 0x102);
		GET_DYNAMIC_GPR(rpatch, 0x103);

		GET_OUTPUT_GPR(rpatch, 0x104, 0x8);
		GET_OUTPUT_GPR(rpatch, 0x105, 0x9);
		GET_OUTPUT_GPR(rpatch, 0x10a, 0x2);
		GET_OUTPUT_GPR(rpatch, 0x10b, 0x3);
		
		
		/* input buffer */
		OP(6, 0x102, AC97_IN_L, 0x40, 0x40);
		OP(6, 0x103, AC97_IN_R, 0x40, 0x40);


		/* Digital In + PCM + MULTI_FRONT-> AC97 out (front speakers)*/
		OP(6, AC97_FRONT_L, 0x100, 0x10c, 0x113);

		CONNECT(MULTI_FRONT_L, AC97_FRONT_L);
		CONNECT(PCM_IN_L, AC97_FRONT_L);
		CONNECT(SPDIF_CD_L, AC97_FRONT_L);

		OP(6, AC97_FRONT_R, 0x101, 0x10e, 0x114);

		CONNECT(MULTI_FRONT_R, AC97_FRONT_R);
		CONNECT(PCM_IN_R, AC97_FRONT_R);
		CONNECT(SPDIF_CD_R, AC97_FRONT_R);

		/* Digital In + PCM + AC97 In + PCM1 + MULTI_REAR --> Rear Channel */ 
		OP(6, 0x104, PCM1_IN_L, 0x100, 0x115);
		OP(6, 0x104, 0x104, 0x10c, 0x102);

		CONNECT(MULTI_REAR_L, ANALOG_REAR_L);
		CONNECT(AC97_IN_L, ANALOG_REAR_L);
		CONNECT(PCM_IN_L, ANALOG_REAR_L);
		CONNECT(SPDIF_CD_L, ANALOG_REAR_L);
		CONNECT(PCM1_IN_L, ANALOG_REAR_L);

		OP(6, 0x105, PCM1_IN_R, 0x101, 0x116);
		OP(6, 0x105, 0x105, 0x10e, 0x103);

		CONNECT(MULTI_REAR_R, ANALOG_REAR_R);
		CONNECT(AC97_IN_R, ANALOG_REAR_R);
		CONNECT(PCM_IN_R, ANALOG_REAR_R);
		CONNECT(SPDIF_CD_R, ANALOG_REAR_R);
		CONNECT(PCM1_IN_R, ANALOG_REAR_R);

		/* Digital In + PCM + AC97 In + MULTI_FRONT --> Digital out */
		OP(6, 0x10b, 0x100, 0x102, 0x10c);
		OP(6, 0x10b, 0x10b, 0x113, 0x40);

		CONNECT(MULTI_FRONT_L, DIGITAL_OUT_L);
		CONNECT(PCM_IN_L, DIGITAL_OUT_L);
		CONNECT(AC97_IN_L, DIGITAL_OUT_L);
		CONNECT(SPDIF_CD_L, DIGITAL_OUT_L);

		OP(6, 0x10a, 0x101, 0x103, 0x10e);
		OP(6, 0x10b, 0x10b, 0x114, 0x40);

		CONNECT(MULTI_FRONT_R, DIGITAL_OUT_R);
		CONNECT(PCM_IN_R, DIGITAL_OUT_R);
		CONNECT(AC97_IN_R, DIGITAL_OUT_R);
		CONNECT(SPDIF_CD_R, DIGITAL_OUT_R);

		/* AC97 In --> ADC Recording Buffer */
		OP(6, ADC_REC_L, 0x102, 0x40, 0x40);

		CONNECT(AC97_IN_L, ADC_REC_L);

		OP(6, ADC_REC_R, 0x103, 0x40, 0x40);

		CONNECT(AC97_IN_R, ADC_REC_R);


		/* fx12:Analog-Center */
		OP(6, ANALOG_CENTER, 0x117, 0x40, 0x40);
		CONNECT(MULTI_CENTER, ANALOG_CENTER);

		/* fx11:Analog-LFE */
		OP(6, ANALOG_LFE, 0x118, 0x40, 0x40);
		CONNECT(MULTI_LFE, ANALOG_LFE);

		/* fx12:Digital-Center */
		OP(6, DIGITAL_CENTER, 0x117, 0x40, 0x40);
		CONNECT(MULTI_CENTER, DIGITAL_CENTER);
		
		/* fx11:Analog-LFE */
		OP(6, DIGITAL_LFE, 0x118, 0x40, 0x40);
		CONNECT(MULTI_LFE, DIGITAL_LFE);
	
		ROUTING_PATCH_END(rpatch);


		// Rear volume control	
		OUTPUT_PATCH_START(patch, "Vol Rear L", 0x8, 0);
		GET_INPUT_GPR(patch, 0x104, 0x8);
		GET_CONTROL_GPR(patch, 0x119, "Vol", 0, 0x7fffffff);

		OP(0, ANALOG_REAR_L, 0x040, 0x104, 0x119);
		OUTPUT_PATCH_END(patch);

		OUTPUT_PATCH_START(patch, "Vol Rear R", 0x9, 0);
		GET_INPUT_GPR(patch, 0x105, 0x9);
		GET_CONTROL_GPR(patch, 0x11a, "Vol", 0, 0x7fffffff);

		OP(0, ANALOG_REAR_R, 0x040, 0x105, 0x11a);
		OUTPUT_PATCH_END(patch);


		//Master volume control on front-digital	
		OUTPUT_PATCH_START(patch, "Vol Master L", 0x2, 1);
		GET_INPUT_GPR(patch, 0x10a, 0x2);
		GET_CONTROL_GPR(patch, 0x108, "Vol", 0, 0x7fffffff);

		OP(0, DIGITAL_OUT_L, 0x040, 0x10a, 0x108);
		OUTPUT_PATCH_END(patch);


		OUTPUT_PATCH_START(patch, "Vol Master R", 0x3, 1);
		GET_INPUT_GPR(patch, 0x10b, 0x3);
		GET_CONTROL_GPR(patch, 0x109, "Vol", 0, 0x7fffffff);

		OP(0, DIGITAL_OUT_R, 0x040, 0x10b, 0x109);
		OUTPUT_PATCH_END(patch);


		/* delimiter patch */
		patch = PATCH(mgr, patch_n);
		patch->code_size = 0;

	
		sblive_writeptr(card, DBG, 0, 0);
	}

	/* FX volume correction and Volume control*/
/*	INPUT_PATCH_START(patch, "Pcm L vol", 0x0, 0);
	GET_OUTPUT_GPR(patch, 0x100, 0x0);
	GET_CONTROL_GPR(patch, 0x106, "Vol", 0, 0x7fffffff);
	GET_DYNAMIC_GPR(patch, 0x112);

	OP(4, 0x112, 0x40, PCM_IN_L, 0x44); //4
	OP(0, 0x100, 0x040, 0x112, 0x106);  //vol
	INPUT_PATCH_END(patch);


	INPUT_PATCH_START(patch, "Pcm R vol", 0x1, 0);
	GET_OUTPUT_GPR(patch, 0x101, 0x1);
	GET_CONTROL_GPR(patch, 0x107, "Vol", 0, 0x7fffffff);
	GET_DYNAMIC_GPR(patch, 0x112);

	OP(4, 0x112, 0x40, PCM_IN_R, 0x44); 
	OP(0, 0x101, 0x040, 0x112, 0x107);

	INPUT_PATCH_END(patch);


	// CD-Digital In Volume control
	INPUT_PATCH_START(patch, "CD-Digital Vol L", 0x12, 0);
	GET_OUTPUT_GPR(patch, 0x10c, 0x12);
	GET_CONTROL_GPR(patch, 0x10d, "Vol", 0, 0x7fffffff);

	OP(0, 0x10c, 0x040, SPDIF_CD_L, 0x10d);
	INPUT_PATCH_END(patch);

	INPUT_PATCH_START(patch, "CD-Digital Vol R", 0x13, 0);
	GET_OUTPUT_GPR(patch, 0x10e, 0x13);
	GET_CONTROL_GPR(patch, 0x10f, "Vol", 0, 0x7fffffff);

	OP(0, 0x10e, 0x040, SPDIF_CD_R, 0x10f);
	INPUT_PATCH_END(patch);

	//Volume Correction for Multi-channel Inputs
	INPUT_PATCH_START(patch, "Multi-Channel Gain", 0x08, 0);
	patch->input=patch->output=0x3F00;

	GET_OUTPUT_GPR(patch, 0x113, MULTI_FRONT_L);
	GET_OUTPUT_GPR(patch, 0x114, MULTI_FRONT_R);
	GET_OUTPUT_GPR(patch, 0x115, MULTI_REAR_L);
	GET_OUTPUT_GPR(patch, 0x116, MULTI_REAR_R);
	GET_OUTPUT_GPR(patch, 0x117, MULTI_CENTER);
	GET_OUTPUT_GPR(patch, 0x118, MULTI_LFE);

	OP(4, 0x113, 0x40, MULTI_FRONT_L, 0x44);
	OP(4, 0x114, 0x40, MULTI_FRONT_R, 0x44);
	OP(4, 0x115, 0x40, MULTI_REAR_L, 0x44);
	OP(4, 0x116, 0x40, MULTI_REAR_R, 0x44);
	OP(4, 0x117, 0x40, MULTI_CENTER, 0x44);
	OP(4, 0x118, 0x40, MULTI_LFE, 0x44);
	
	INPUT_PATCH_END(patch);


	//Routing patch start
	ROUTING_PATCH_START(rpatch, "Routing");
	GET_INPUT_GPR(rpatch, 0x100, 0x0);
	GET_INPUT_GPR(rpatch, 0x101, 0x1);
	GET_INPUT_GPR(rpatch, 0x10c, 0x12);
	GET_INPUT_GPR(rpatch, 0x10e, 0x13);
	GET_INPUT_GPR(rpatch, 0x113, MULTI_FRONT_L);
	GET_INPUT_GPR(rpatch, 0x114, MULTI_FRONT_R);
	GET_INPUT_GPR(rpatch, 0x115, MULTI_REAR_L);
	GET_INPUT_GPR(rpatch, 0x116, MULTI_REAR_R);
	GET_INPUT_GPR(rpatch, 0x117, MULTI_CENTER);
	GET_INPUT_GPR(rpatch, 0x118, MULTI_LFE);

	GET_DYNAMIC_GPR(rpatch, 0x102);
	GET_DYNAMIC_GPR(rpatch, 0x103);

	GET_OUTPUT_GPR(rpatch, 0x104, 0x8);
	GET_OUTPUT_GPR(rpatch, 0x105, 0x9);
	GET_OUTPUT_GPR(rpatch, 0x10a, 0x2);
	GET_OUTPUT_GPR(rpatch, 0x10b, 0x3);


	// input buffer 
	OP(6, 0x102, AC97_IN_L, 0x40, 0x40);
	OP(6, 0x103, AC97_IN_R, 0x40, 0x40);


	// Digital In + PCM + MULTI_FRONT-> AC97 out (front speakers)
	OP(6, AC97_FRONT_L, 0x100, 0x10c, 0x113);

	CONNECT(MULTI_FRONT_L, AC97_FRONT_L);
	CONNECT(PCM_IN_L, AC97_FRONT_L);
	CONNECT(SPDIF_CD_L, AC97_FRONT_L);

	OP(6, AC97_FRONT_R, 0x101, 0x10e, 0x114);

	CONNECT(MULTI_FRONT_R, AC97_FRONT_R);
	CONNECT(PCM_IN_R, AC97_FRONT_R);
	CONNECT(SPDIF_CD_R, AC97_FRONT_R);

	// Digital In + PCM + AC97 In + PCM1 + MULTI_REAR --> Rear Channel 
	OP(6, 0x104, PCM1_IN_L, 0x100, 0x115);
	OP(6, 0x104, 0x104, 0x10c, 0x102);

	CONNECT(MULTI_REAR_L, ANALOG_REAR_L);
	CONNECT(AC97_IN_L, ANALOG_REAR_L);
	CONNECT(PCM_IN_L, ANALOG_REAR_L);
	CONNECT(SPDIF_CD_L, ANALOG_REAR_L);
	CONNECT(PCM1_IN_L, ANALOG_REAR_L);

	OP(6, 0x105, PCM1_IN_R, 0x101, 0x116);
	OP(6, 0x105, 0x105, 0x10e, 0x103);

	CONNECT(MULTI_REAR_R, ANALOG_REAR_R);
	CONNECT(AC97_IN_R, ANALOG_REAR_R);
	CONNECT(PCM_IN_R, ANALOG_REAR_R);
	CONNECT(SPDIF_CD_R, ANALOG_REAR_R);
	CONNECT(PCM1_IN_R, ANALOG_REAR_R);

	// Digital In + PCM + AC97 In + MULTI_FRONT --> Digital out 
	OP(6, 0x10a, 0x100, 0x102, 0x10c);
	OP(6, 0x10a, 0x10a, 0x113, 0x40);

	CONNECT(MULTI_FRONT_L, DIGITAL_OUT_L);
	CONNECT(PCM_IN_L, DIGITAL_OUT_L);
	CONNECT(AC97_IN_L, DIGITAL_OUT_L);
	CONNECT(SPDIF_CD_L, DIGITAL_OUT_L);

	OP(6, 0x10b, 0x101, 0x103, 0x10e);
	OP(6, 0x10b, 0x10b, 0x114, 0x40);

	CONNECT(MULTI_FRONT_R, DIGITAL_OUT_R);
	CONNECT(PCM_IN_R, DIGITAL_OUT_R);
	CONNECT(AC97_IN_R, DIGITAL_OUT_R);
	CONNECT(SPDIF_CD_R, DIGITAL_OUT_R);

	// AC97 In --> ADC Recording Buffer 
	OP(6, ADC_REC_L, 0x102, 0x40, 0x40);

	CONNECT(AC97_IN_L, ADC_REC_L);

	OP(6, ADC_REC_R, 0x103, 0x40, 0x40);

	CONNECT(AC97_IN_R, ADC_REC_R);


	// fx12:Analog-Center 
	OP(6, ANALOG_CENTER, 0x117, 0x40, 0x40);
	CONNECT(MULTI_CENTER, ANALOG_CENTER);

	// fx11:Analog-LFE 
	OP(6, ANALOG_LFE, 0x118, 0x40, 0x40);
	CONNECT(MULTI_LFE, ANALOG_LFE);

	// fx12:Digital-Center 
	OP(6, DIGITAL_CENTER, 0x117, 0x40, 0x40);
	CONNECT(MULTI_CENTER, DIGITAL_CENTER);

	// fx11:Analog-LFE 
	OP(6, DIGITAL_LFE, 0x118, 0x40, 0x40);
	CONNECT(MULTI_LFE, DIGITAL_LFE);
	
	ROUTING_PATCH_END(rpatch);


	// Rear volume control
	OUTPUT_PATCH_START(patch, "Vol Rear L", 0x8, 0);
	GET_INPUT_GPR(patch, 0x104, 0x8);
	GET_CONTROL_GPR(patch, 0x119, "Vol", 0, 0x7fffffff);

	OP(0, ANALOG_REAR_L, 0x040, 0x104, 0x119);
	OUTPUT_PATCH_END(patch);


	OUTPUT_PATCH_START(patch, "Vol Rear R", 0x9, 0);
	GET_INPUT_GPR(patch, 0x105, 0x9);
	GET_CONTROL_GPR(patch, 0x11a, "Vol", 0, 0x7fffffff);

	OP(0, ANALOG_REAR_R, 0x040, 0x105, 0x11a);
	OUTPUT_PATCH_END(patch);


	//Master volume control on front-digital
	OUTPUT_PATCH_START(patch, "Vol Master L", 0x2, 1);
	GET_INPUT_GPR(patch, 0x10a, 0x2);
	GET_CONTROL_GPR(patch, 0x108, "Vol", 0, 0x7fffffff);

	OP(0, DIGITAL_OUT_L, 0x040, 0x10a, 0x108);
	OUTPUT_PATCH_END(patch);


	OUTPUT_PATCH_START(patch, "Vol Master R", 0x3, 1);
	GET_INPUT_GPR(patch, 0x10b, 0x3);
	GET_CONTROL_GPR(patch, 0x109, "Vol", 0, 0x7fffffff);

	OP(0, DIGITAL_OUT_R, 0x040, 0x10b, 0x109);
	OUTPUT_PATCH_END(patch);


	// delimiter patch 
	patch = PATCH(mgr, patch_n);
	patch->code_size = 0;

	sblive_writeptr(card, DBG, 0, 0);*/

	spinlock_init(&mgr->lock,"fxmgr_spinlock");

	//Master volume
	mgr->ctrl_gpr[SOUND_MIXER_VOLUME][0] = 8;
	mgr->ctrl_gpr[SOUND_MIXER_VOLUME][1] = 9;

	left = card->ac97.mixer_state[SOUND_MIXER_VOLUME] & 0xff;
	right = (card->ac97.mixer_state[SOUND_MIXER_VOLUME] >> 8) & 0xff;

	emu10k1_set_volume_gpr(card, 8, left, 1 << card->ac97.bit_resolution);
	emu10k1_set_volume_gpr(card, 9, right, 1 << card->ac97.bit_resolution);

	//Rear volume
	mgr->ctrl_gpr[ SOUND_MIXER_OGAIN ][0] = 0x19;
	mgr->ctrl_gpr[ SOUND_MIXER_OGAIN ][1] = 0x1a;

	left = right = 67;
	card->ac97.mixer_state[SOUND_MIXER_OGAIN] = (right << 8) | left;

	card->ac97.supported_mixers |= SOUND_MASK_OGAIN;
	card->ac97.stereo_mixers |= SOUND_MASK_OGAIN;

	emu10k1_set_volume_gpr(card, 0x19, left, VOL_5BIT);
	emu10k1_set_volume_gpr(card, 0x1a, right, VOL_5BIT);

	//PCM Volume
	mgr->ctrl_gpr[SOUND_MIXER_PCM][0] = 6;
	mgr->ctrl_gpr[SOUND_MIXER_PCM][1] = 7;

	left = card->ac97.mixer_state[SOUND_MIXER_PCM] & 0xff;
	right = (card->ac97.mixer_state[SOUND_MIXER_PCM] >> 8) & 0xff;

	emu10k1_set_volume_gpr(card, 6, left, VOL_5BIT);
	emu10k1_set_volume_gpr(card, 7, right, VOL_5BIT);

	//CD-Digital Volume
	mgr->ctrl_gpr[SOUND_MIXER_DIGITAL1][0] = 0xd;
	mgr->ctrl_gpr[SOUND_MIXER_DIGITAL1][1] = 0xf;

	left = right = 67;
	card->ac97.mixer_state[SOUND_MIXER_DIGITAL1] = (right << 8) | left; 

	card->ac97.supported_mixers |= SOUND_MASK_DIGITAL1;
	card->ac97.stereo_mixers |= SOUND_MASK_DIGITAL1;

	emu10k1_set_volume_gpr(card, 0xd, left, VOL_5BIT);
	emu10k1_set_volume_gpr(card, 0xf, right, VOL_5BIT);

	//hard wire the ac97's pcm, pcm volume is done above using dsp code.
	if (card->is_audigy)
		//for Audigy, we mute it and use the philips 6 channel DAC instead
		emu10k1_ac97_write(&card->ac97, 0x18, 0x8000);
	else
		//For the Live we hardwire it to full volume
		emu10k1_ac97_write(&card->ac97, 0x18, 0x0);

	card->ac97_supported_mixers &= ~SOUND_MASK_PCM;
	card->ac97_stereo_mixers &= ~SOUND_MASK_PCM;

	//set Igain to 0dB by default, maybe consider hardwiring it here.
	emu10k1_ac97_write(&card->ac97, AC97_RECORD_GAIN, 0x0000);
	card->ac97.mixer_state[SOUND_MIXER_IGAIN] = 0x101; 

	return 0;
}

static int hw_init(struct emu10k1_card *card)
{
	int nCh;
	uint32 pagecount; /* tmp */
	int ret;

	/* Disable audio and lock cache */
	emu10k1_writefn0(card, HCFG, HCFG_LOCKSOUNDCACHE | HCFG_LOCKTANKCACHE_MASK | HCFG_MUTEBUTTONENABLE);

	/* Reset recording buffers */
	sblive_writeptr_tag(card, 0,
			    MICBS, ADCBS_BUFSIZE_NONE,
			    MICBA, 0,
			    FXBS, ADCBS_BUFSIZE_NONE,
			    FXBA, 0,
			    ADCBS, ADCBS_BUFSIZE_NONE,
			    ADCBA, 0,
			    TAGLIST_END);

	/* Disable channel interrupt */
	emu10k1_writefn0(card, INTE, 0);
	sblive_writeptr_tag(card, 0,
			    CLIEL, 0,
			    CLIEH, 0,
			    SOLEL, 0,
			    SOLEH, 0,
			    TAGLIST_END);

	/* Init envelope engine */
	for (nCh = 0; nCh < NUM_G; nCh++) {
		sblive_writeptr_tag(card, nCh,
				    DCYSUSV, 0,
				    IP, 0,
				    VTFT, 0xffff,
				    CVCF, 0xffff,
				    PTRX, 0,
				    //CPF, 0,
				    CCR, 0,

				    PSST, 0,
				    DSL, 0x10,
				    CCCA, 0,
				    Z1, 0,
				    Z2, 0,
				    FXRT, 0xd01c0000,

				    ATKHLDM, 0,
				    DCYSUSM, 0,
				    IFATN, 0xffff,
				    PEFE, 0,
				    FMMOD, 0,
				    TREMFRQ, 24,	/* 1 Hz */
				    FM2FRQ2, 24,	/* 1 Hz */
				    TEMPENV, 0,

				    /*** These are last so OFF prevents writing ***/
				    LFOVAL2, 0,
				    LFOVAL1, 0,
				    ATKHLDV, 0,
				    ENVVOL, 0,
				    ENVVAL, 0,
                                    TAGLIST_END);
		sblive_writeptr(card, CPF, nCh, 0);

		/*
		  Audigy FXRT initialization
		  reversed eng'd, may not be accurate.
		 */
		if (card->is_audigy) {
			sblive_writeptr_tag(card,nCh,
					    0x4c,0x0,
					    0x4d,0x0,
					    0x4e,0x0,
					    0x4f,0x0,
					    A_FXRT1, 0x3f3f3f3f,
					    A_FXRT2, 0x3f3f3f3f,
					    A_SENDAMOUNTS, 0,
					    TAGLIST_END);
		}
	}
	

	/*
	 ** Init to 0x02109204 :
	 ** Clock accuracy    = 0     (1000ppm)
	 ** Sample Rate       = 2     (48kHz)
	 ** Audio Channel     = 1     (Left of 2)
	 ** Source Number     = 0     (Unspecified)
	 ** Generation Status = 1     (Original for Cat Code 12)
	 ** Cat Code          = 12    (Digital Signal Mixer)
	 ** Mode              = 0     (Mode 0)
	 ** Emphasis          = 0     (None)
	 ** CP                = 1     (Copyright unasserted)
	 ** AN                = 0     (Digital audio)
	 ** P                 = 0     (Consumer)
	 */

	sblive_writeptr_tag(card, 0,

			    /* SPDIF0 */
			    SPCS0, (SPCS_CLKACCY_1000PPM | 0x002000000 |
				    SPCS_CHANNELNUM_LEFT | SPCS_SOURCENUM_UNSPEC | SPCS_GENERATIONSTATUS | 0x00001200 | SPCS_EMPHASIS_NONE | SPCS_COPYRIGHT),

			    /* SPDIF1 */
			    SPCS1, (SPCS_CLKACCY_1000PPM | 0x002000000 |
				    SPCS_CHANNELNUM_LEFT | SPCS_SOURCENUM_UNSPEC | SPCS_GENERATIONSTATUS | 0x00001200 | SPCS_EMPHASIS_NONE | SPCS_COPYRIGHT),

			    /* SPDIF2 & SPDIF3 */
			    SPCS2, (SPCS_CLKACCY_1000PPM | 0x002000000 |
				    SPCS_CHANNELNUM_LEFT | SPCS_SOURCENUM_UNSPEC | SPCS_GENERATIONSTATUS | 0x00001200 | SPCS_EMPHASIS_NONE | SPCS_COPYRIGHT),

			    TAGLIST_END);

	ret = fx_init(card);		/* initialize effects engine */
	if (ret < 0)
		return ret;

	card->tankmem.size = 0;

	card->virtualpagetable.size = MAXPAGES * sizeof(uint32);

	card->virtualpagetable.addr = alloc_dma_mem(card->pci_dev, card->virtualpagetable.size, &card->virtualpagetable.dma_handle);
//	card->virtualpagetable.addr = kmalloc(card->virtualpagetable.size+PAGE_SIZE, MEMF_KERNEL);
//	card->virtualpagetable.addr = get_free_pages( 1, 0 );
	if (card->virtualpagetable.addr == NULL) {
		ERROR();
		ret = -ENOMEM;
		goto err0;
	}
//	card->virtualpagetable.addr = (void*)PAGE_ALIGN((uint32)(card->virtualpagetable.addr));
//	card->virtualpagetable.dma_handle = card->virtualpagetable.addr;

	card->silentpage.size = EMUPAGESIZE;

	card->silentpage.addr = alloc_dma_mem(card->pci_dev, card->silentpage.size, &card->silentpage.dma_handle);
	if (card->silentpage.addr == NULL) {
		ERROR();
		ret = -ENOMEM;
		goto err1;
	}

	for (pagecount = 0; pagecount < MAXPAGES; pagecount++)
		((uint32 *) card->virtualpagetable.addr)[pagecount] = ((uint32)card->silentpage.dma_handle * 2) | pagecount;

	/* Init page table & tank memory base register */
	sblive_writeptr_tag(card, 0,
			    PTB, (uint32)card->virtualpagetable.dma_handle,
			    TCB, 0,
			    TCBS, 0,
			    TAGLIST_END);

	for (nCh = 0; nCh < NUM_G; nCh++) {
		sblive_writeptr_tag(card, nCh,
				    MAPA, MAP_PTI_MASK |( (uint32)card->silentpage.dma_handle * 2),
				    MAPB, MAP_PTI_MASK |( (uint32)card->silentpage.dma_handle * 2),
				    TAGLIST_END);
	}

	/* Hokay, now enable the AUD bit */
	/* Enable Audio = 1 */
	/* Mute Disable Audio = 0 */
	/* Lock Tank Memory = 1 */
	/* Lock Sound Memory = 0 */
	/* Auto Mute = 1 */

	if (card->model == 0x20 || card->model == 0xc400 ||
	  (card->model == 0x21 && card->chiprev < 6))
	        emu10k1_writefn0(card, HCFG, HCFG_AUDIOENABLE  | HCFG_LOCKTANKCACHE_MASK | HCFG_AUTOMUTE);
	else
		emu10k1_writefn0(card, HCFG, HCFG_AUDIOENABLE  | HCFG_LOCKTANKCACHE_MASK | HCFG_AUTOMUTE | HCFG_JOYENABLE);

	/* Enable Vol_Ctrl irqs */
	emu10k1_irq_enable(card, INTE_VOLINCRENABLE | INTE_VOLDECRENABLE | INTE_MUTEENABLE | INTE_FXDSPENABLE);

	/* FIXME: TOSLink detection */
	card->has_toslink = 0;

	return 0;

err1:
	free_dma_mem(card->pci_dev, card->virtualpagetable.size, card->virtualpagetable.addr, card->virtualpagetable.dma_handle);
err0:
	fx_cleanup(&card->mgr);

	return ret;
}

static int emu10k1_init(struct emu10k1_card *card)
{
	/* Init Card */
	if (hw_init(card) < 0)
		return -1;

	voice_init(card);
	timer_init(card);
	addxmgr_init(card);

	DPD(2, "  hw control register -> %#x\n", emu10k1_readfn0(card, HCFG));

	return 0;
}

static void emu10k1_cleanup(struct emu10k1_card *card)
{
	int ch;

	emu10k1_writefn0(card, INTE, 0);

	/** Shutdown the chip **/
	for (ch = 0; ch < NUM_G; ch++)
		sblive_writeptr(card, DCYSUSV, ch, 0);

	for (ch = 0; ch < NUM_G; ch++)
	{
		sblive_writeptr_tag(card, ch,
				    VTFT, 0,
				    CVCF, 0,
				    PTRX, 0,
				    //CPF, 0,
				    TAGLIST_END);
		sblive_writeptr(card, CPF, ch, 0);
	}

	/* Disable audio and lock cache */
	emu10k1_writefn0(card, HCFG, HCFG_LOCKSOUNDCACHE | HCFG_LOCKTANKCACHE_MASK | HCFG_MUTEBUTTONENABLE);

	sblive_writeptr_tag(card, 0,
                            PTB, 0,

			    /* Reset recording buffers */
			    MICBS, ADCBS_BUFSIZE_NONE,
			    MICBA, 0,
			    FXBS, ADCBS_BUFSIZE_NONE,
			    FXBA, 0,
			    FXWC, 0,
			    ADCBS, ADCBS_BUFSIZE_NONE,
			    ADCBA, 0,
			    TCBS, 0,
			    TCB, 0,
			    DBG, 0x8000,

			    /* Disable channel interrupt */
			    CLIEL, 0,
			    CLIEH, 0,
			    SOLEL, 0,
			    SOLEH, 0,
			    TAGLIST_END);


	free_dma_mem(card->pci_dev, card->virtualpagetable.size, card->virtualpagetable.addr, card->virtualpagetable.dma_handle);
	free_dma_mem(card->pci_dev, card->silentpage.size, card->silentpage.addr, card->silentpage.dma_handle);
	
	if(card->tankmem.size != 0)
		free_dma_mem(card->pci_dev, card->tankmem.size, card->tankmem.addr, card->tankmem.dma_handle);

	/* release patch storage memory */
	fx_cleanup(&card->mgr);
}

/* Driver initialization routine */
static int emu10k1_probe(PCI_Info_s *pci_dev,int device_id)
{
	struct emu10k1_card *card;
	uint32 subsysvid;
	int ret;

	if ((card = kmalloc(sizeof(struct emu10k1_card), MEMF_KERNEL)) == NULL)
	{
                printk("emu10k1: out of memory\n");
                return -ENOMEM;
	}
	memset(card, 0, sizeof(struct emu10k1_card));

	card->iobase = pci_dev->u.h0.nBase0 & PCI_ADDRESS_MEMORY_32_MASK;

	card->irq = pci_dev->u.h0.nInterruptLine;
	card->pci_dev = pci_dev;

	/* Reserve IRQ Line */
	card->irq_handle=request_irq(card->irq, emu10k1_interrupt, NULL, SA_SHIRQ,"emu10k1", card);

	if (!card->irq_handle)
	{
		printk("emu10k1: IRQ in use\n");
		ret = -EBUSY;
		goto err_irq;
	}

	pci_read_config_byte(pci_dev, PCI_REVISION, &card->chiprev);
	pci_read_config_word(pci_dev, PCI_SUBSYSTEM_ID, &card->model);

	if (pci_dev->nDeviceID == PCI_DEVICE_ID_CREATIVE_AUDIGY)
		card->is_audigy = 1;

	printk("emu10k1: %s rev %d model 0x%x found, IO at 0x%04lx, IRQ %d\n",
		card_names[card->is_audigy], card->chiprev, card->model, card->iobase, card->irq);

	pci_read_config_dword(pci_dev, PCI_SUBSYSTEM_VENDOR_ID, &subsysvid);
	card->is_aps = (subsysvid == EMU_APS_SUBID);

	spinlock_init(&card->lock,"emu10k1_card_spinlock");
	card->open_sem=create_semaphore("emu10k1_card_opensem",0,SEM_RECURSIVE);
	card->open_mode = 0;
	card->open_wait=create_semaphore("emu10k1_wait_queue",1,SEM_RECURSIVE);
	card->dev_id=device_id;

	ret = emu10k1_audio_init(card, pci_dev->nHandle);
	if(ret < 0)
	{
                printk("emu10k1: cannot initialize audio devices\n");
                goto err_audio;
	}

	ret = emu10k1_mixer_init(card, pci_dev->nHandle);
	if(ret < 0)
	{
		printk("emu10k1: cannot initialize AC97 codec\n");
		goto err_mixer;
	}

	ret = emu10k1_init(card);
	if (ret < 0)
	{
		printk("emu10k1: cannot initialize device\n");
		goto err_emu10k1_init;
	}

	list_add(&card->list, &emu10k1_devs);

	sblive_card=card;

	return 0;

err_emu10k1_init:
	emu10k1_mixer_cleanup(card);

err_mixer:
	emu10k1_audio_cleanup(card);

err_audio:
	release_irq(card->irq, card->irq_handle);

err_irq:
	kfree(card);

	return ret;
}

static void emu10k1_remove(void)
{
	struct emu10k1_card *card = sblive_card;

	list_del(&card->list);

	emu10k1_cleanup(card);
	emu10k1_mixer_cleanup(card);
	emu10k1_audio_cleanup(card);	
	release_irq(card->irq, card->irq_handle);
	kfree(card);
}

status_t device_init(int nDeviceID)
{
	int currentdev;
	int devfound=0;
	PCI_Info_s pcidevice;
	PCI_bus_s* psBus = get_busmanager( PCI_BUS_NAME, PCI_BUS_VERSION );
	if( psBus == NULL )
		return( -ENODEV );

	for(currentdev=0;psBus->get_pci_info(&pcidevice,currentdev)==0;currentdev++)
	{
		if(pcidevice.nVendorID == PCI_VENDOR_ID_CREATIVE && ( pcidevice.nDeviceID == PCI_DEVICE_ID_CREATIVE_EMU10K1 || pcidevice.nDeviceID == PCI_DEVICE_ID_CREATIVE_AUDIGY ) )
		{
			if(emu10k1_probe(&pcidevice,nDeviceID)!=0)
			{
				printk("emu10k1: Failed to initialise SBLive!\n");
			}
			else
			{
				if( claim_device( nDeviceID, pcidevice.nHandle, "EMU10K", DEVICE_AUDIO ) == 0 ) {
					printk("emu10k1: SBLive! initialised\n");
					devfound=1;
				}
			}
		}
	}

	if(devfound==0) {
		disable_device( nDeviceID );
		return -ENODEV;
	}

	return EOK;
}

status_t device_uninit(int nDeviceID)
{
	emu10k1_remove();
	return EOK;
}







