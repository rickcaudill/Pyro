# Makefile for ported software.

BACK=`pwd`
URL = http://www.linuxmotors.com/sdllopan/downloads/
NAME = sdllopan
VERSION = 10
DOCS = Changelog README

dist: build inst doc checksum package clean

build:
	wget -N $(URL)$(NAME)-$(VERSION).tgz && sync && \
	tar -xzkvf $(NAME)-$(VERSION).tgz && sync && \
	cd $(NAME)-$(VERSION) && \
	make && sync && \
	cd ..

inst:
	addattrib $(NAME)-$(VERSION)/lopan os::Category Games
	mkdir -p $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Applications/$(NAME)
	cp -f $(NAME)-$(VERSION)/lopan $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Applications/$(NAME)/
	cp -rf $(NAME)-$(VERSION)/data $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Applications/$(NAME)/
	sync

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Documentation/Games/$(NAME)
	for DOC in $(DOCS);do \
		cp -f $(NAME)-$(VERSION)/$$DOC $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Documentation/Games/$(NAME)/; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Documentation/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/plain"; \
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
	cp -f $(NAME)-$(VERSION).tgz $(PYRO_BINARY_DESTINATION)Source/Extra/ && \
	cd $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION) && \
	zip -ry9 $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION) && \
	sync

clean:
	rm -rf $(NAME)-$(VERSION)
	rm -f $(NAME)-$(VERSION).tgz
	sync
