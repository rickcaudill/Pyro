# Makefile for ported software.

BACK=`pwd`
URL = http://grayskygames.com/sokoban/
NAME = syasokoban
VERSION = 2.0.1
DOCSVIDEO = http://grayskygames.com/sokoban/syasokobantour2.ogv
DOCS = *.txt

dist: build inst doc checksum package clean

build:
	wget -N $(URL)$(NAME)-$(VERSION).tar.gz && sync && \
	wget -N $(DOCSVIDEO) && sync && \
	tar -xzkvf $(NAME)-$(VERSION).tar.gz && sync && \
	cd $(NAME)-$(VERSION) && \
	make && sync && \
	cd ..

inst:
	addattrib $(NAME)-$(VERSION)/$(NAME) os::Category Games
	addattrib $(NAME)-$(VERSION)/$(NAME) os::Icon data/sokoban/SYASokoban.png
	mkdir -p $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Applications/$(NAME)-$(VERSION)
	cp -f $(NAME)-$(VERSION)/$(NAME) $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Applications/$(NAME)-$(VERSION)/
	cp -rf $(NAME)-$(VERSION)/data $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Applications/$(NAME)-$(VERSION)/
	sync

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Documentation/Games/$(NAME)-$(VERSION)
	for DOC in $(DOCS);do \
		cp -f $(NAME)-$(VERSION)/$$DOC $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Documentation/Games/$(NAME)-$(VERSION)/; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Documentation/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/plain"; \
		fi; \
	done;
	cp -f syasokobantour2.ogv $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Documentation/Games/$(NAME)-$(VERSION)/
	addattrib $(PYRO_BINARY_DESTINATION)Extra/Ported/Games/$(NAME)-$(VERSION)/Documentation/Games/$(NAME)-$(VERSION)/syasokobantour2.ogv "os::MimeType" "video/ogg"
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
