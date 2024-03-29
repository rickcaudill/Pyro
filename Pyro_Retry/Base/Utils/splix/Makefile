# Makefile for ported software.

BACK=`pwd`
URL = http://sourceforge.net/projects/splix/files/splix/
NAME = splix
VERSION = 2.0.0
ARGS =	DESTDIR=$(PYRO_BINARY_DESTINATION)Base/Utils/$(NAME)-$(VERSION)/NIX/$(NAME)
DESC = A set of CUPS printer drivers for SPL (Samsung Printer Language) printers.
DOCS = AUTHORS ChangeLog COPYING INSTALL README THANKS TODO

dist: build doc update checksum package clean

build:
	wget -N $(URL)$(VERSION)/$(NAME)-$(VERSION).tar.bz2/download && sync && \
	tar -xjkvf $(NAME)-$(VERSION).tar.bz2 && sync && \
	cd $(NAME)-$(VERSION) && \
	make install $(ARGS) && sync && \
	mkdir -p $(PYRO_BINARY_DESTINATION)Base/Utils/$(NAME)-$(VERSION)/NIX/cups && \
	mv $(PYRO_BINARY_DESTINATION)Base/Utils/$(NAME)-$(VERSION)/NIX/$(NAME)/system/resources/cups/1.3.4/* $(PYRO_BINARY_DESTINATION)Base/Utils/$(NAME)-$(VERSION)/NIX/cups/ && \
	rm -rf $(PYRO_BINARY_DESTINATION)Base/Utils/$(NAME)-$(VERSION)/NIX/$(NAME)/system && \
	cd ..

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Base/Utils/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)
	for DOC in $(DOCS);do \
		cp -f $(NAME)-$(VERSION)/$$DOC $(PYRO_BINARY_DESTINATION)Base/Utils/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)/; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Base/Utils/$(NAME)-$(VERSION)/Documentation/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/plain"; \
		fi; \
	done;
	sync

update:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Base/Utils/$(NAME)-$(VERSION)/NIX/`; do \
		if [ `file --brief --mime-type $$FILES | grep -c "text" -` != 0 ]; then \
			sed -i "s,$(PYRO_BINARY_DESTINATION)Base/Utils/$(NAME)-$(VERSION),/boot,g" $$FILES; \
		fi; \
	done;
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Base/Utils/$(NAME)-$(VERSION)/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

package:
	echo $(DESC) > $(TEMP)/zip_comment && \
	mkdir -p $(PYRO_BINARY_DESTINATION)Source/Base && \
	cp -f $(NAME)-$(VERSION).tar.bz2 $(PYRO_BINARY_DESTINATION)Source/Base/ && \
	cd $(PYRO_BINARY_DESTINATION)Base/Utils/$(NAME)-$(VERSION) && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Base/Utils/$(NAME)-$(VERSION).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Base/Utils/$(NAME)-$(VERSION) && \
	sync

clean:
	rm -rf $(NAME)-$(VERSION)
	rm -f $(NAME)-$(VERSION).tar.bz2
	sync
