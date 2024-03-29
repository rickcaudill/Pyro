# Makefile for ported software.

BACK=`pwd`
URL = http://sourceforge.net/projects/hannah/files/hannah/hannah-
NAME = HelpHannahsHorse
VERSION = 1.0
ALTNAME = hhh
ALTVERSION = 2008-03-29
ARGS = $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Applications/$(NAME)
DOCS = CHANGELOG INSTALL LICENSE README TODO

dist: build doc update checksum package clean

build:
	wget -N $(URL)$(VERSION)/$(ALTNAME)-$(VERSION).tar.gz/download && sync && \
	tar -xzkvf $(ALTNAME)-$(VERSION).tar.gz && sync && \
	cd $(ALTNAME)-$(ALTVERSION) && \
	sed -i "s,DATA_INSTALL_DIR=/usr/local/games/hhh,DATA_INSTALL_DIR=$(ARGS),g" Makefile && \
	sed -i "s,BIN_DIR=/usr/local/bin,BIN_DIR=$(ARGS),g" Makefile && \
	make && make install && sync && \
	cd ..

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Applications/$(NAME)/Documentation
	for DOC in $(DOCS);do \
		cp -f $(ALTNAME)-$(ALTVERSION)/$$DOC $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Applications/$(NAME)/Documentation/; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Applications/$(NAME)/Documentation/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/plain"; \
		fi; \
	done;
	sync

update:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Applications/$(NAME)/`; do \
		if [ `file --brief --mime-type $$FILES | grep -c "text" -` != 0 ]; then \
			sed -i "s,$(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION),/boot,g" $$FILES; \
		fi; \
	done;
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

package:
	mkdir -p $(PYRO_BINARY_DESTINATION)Source/Extra && \
	cp -f $(ALTNAME)-$(VERSION).tar.gz $(PYRO_BINARY_DESTINATION)Source/Extra/ && \
	cd $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION) && \
	zip -ry9 $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION) && \
	sync

clean:
	rm -rf $(ALTNAME)-$(ALTVERSION)
	rm -f $(ALTNAME)-$(VERSION).tar.gz
	sync
