CFLAGS = -Wall -c
CPPFLAGS = -Wall -c

OBJS = main.o editview.o controlpanel.o iconview.o winicon.o bitmapscale.o
EXE	= iconedit

CC	= gcc
LL	= gcc

OBJDIR	:= objs

include $(PYRO_COMMON)/makefile.rules

$(OBJDIR)/$(EXE) : $(OBJDIR) $(OBJS)
	$(LL) $(OBJS) -latheos -lstdc++ -lm -o $@

$(OBJDIR):
	mkdir $(OBJDIR)

deps: $(OBJDIR) $(DEPS)

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

BACK=`pwd`

dist: $(OBJDIR)/$(EXE) inst package clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Extra/Native/Utils/$(EXE)/Applications/$(EXE)
	cp -f $(OBJDIR)/$(EXE) $(PYRO_BINARY_DESTINATION)Extra/Native/Utils/$(EXE)/Applications/$(EXE)/
	strip --strip-all $(PYRO_BINARY_DESTINATION)Extra/Native/Utils/$(EXE)/Applications/$(EXE)/$(EXE)
	addattrib $(PYRO_BINARY_DESTINATION)Extra/Native/Utils/$(EXE)/Applications/$(EXE)/$(EXE) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)Extra/Native/Utils/$(EXE)/Applications/$(EXE)/$(EXE) | cut -c1-32`
	sync

package:
	cd $(PYRO_BINARY_DESTINATION)Extra/Native/Utils/$(EXE) && \
	zip -ry9 $(PYRO_BINARY_DESTINATION)Extra/Native/Utils/$(EXE).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Extra/Native/Utils/$(EXE) && \
	sync

clean:
	rm -rf $(OBJDIR)
