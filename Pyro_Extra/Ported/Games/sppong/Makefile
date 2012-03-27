# Makefile for ported software.

BACK=`pwd`
URL = http://www.iissam.com/sdl/
NAME = sppong
VERSION = 0.1

dist: build inst checksum package clean

build:
	wget -N $(URL)$(NAME)-$(VERSION).tar.gz && sync && \
	tar -xzkvf $(NAME)-$(VERSION).tar.gz && sync && \
	cd $(NAME)-$(VERSION) && \
	rm -f ./Sppong && \
	rm -f ./data/.directory && \
	make && sync && \
	cd ..

inst:
	addattrib $(NAME)-$(VERSION)/Sppong os::Category Games
	mkdir -p $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Applications/$(NAME)
	cp -f $(NAME)-$(VERSION)/Sppong $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Applications/$(NAME)/
	cp -rf $(NAME)-$(VERSION)/data $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Applications/$(NAME)/
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
	cp -f $(NAME)-$(VERSION).tar.gz $(PYRO_BINARY_DESTINATION)Source/Extra/ && \
	cd $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION) && \
	zip -ry9 $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION) && \
	sync

clean:
	rm -rf $(NAME)-$(VERSION)
	rm -f $(NAME)-$(VERSION).tar.gz
	sync
