# Makefile for ported software.

BACK=`pwd`
URL = http://grayskygames.com/sokoban/version1/
NAME = syasokoban
DOWNLOADNAME = SYASokobanSrc
VERSION = 1.02
DOCS = *.txt

dist: build inst doc checksum package clean

build:
	wget -N $(URL)$(DOWNLOADNAME).zip && sync && \
	unzip -n $(DOWNLOADNAME).zip && sync && \
	cd $(DOWNLOADNAME) && \
	make && sync && \
	cd ..

inst:
	addattrib $(DOWNLOADNAME)/$(NAME) os::Category Games
	mkdir -p $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Applications/$(NAME)-$(VERSION)
	cp -f $(DOWNLOADNAME)/$(NAME) $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Applications/$(NAME)-$(VERSION)/
	cp -rf $(DOWNLOADNAME)/data $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Applications/$(NAME)-$(VERSION)/
	sync

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Documentation/Games/$(NAME)-$(VERSION)
	for DOC in $(DOCS);do \
		cp -f $(DOWNLOADNAME)/$$DOC $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Documentation/Games/$(NAME)-$(VERSION)/; \
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
	cp -f $(DOWNLOADNAME).zip $(PYRO_BINARY_DESTINATION)Source/Extra/ && \
	cd $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION) && \
	zip -ry9 $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION) && \
	sync

clean:
	rm -rf $(DOWNLOADNAME)
	rm -f $(DOWNLOADNAME).zip
	sync
