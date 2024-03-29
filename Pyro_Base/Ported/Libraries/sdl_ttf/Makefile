# Makefile for ported software.

BACK=`pwd`
URL = http://www.libsdl.org/projects/SDL_ttf/release/
NAME = SDL_ttf
VERSION = 2.0.10
ARGS =	--prefix=$(PYRO_BINARY_DESTINATION)Base/Libraries/$(NAME)-$(VERSION)/NIX/$(NAME)
DESC = A sample library which allows you to use TrueType fonts in SDL applications.
DOCS = CHANGES COPYING README

dist: build doc update checksum package clean

build:
	wget -N $(URL)$(NAME)-$(VERSION).tar.gz && sync && \
	tar -xzkvf $(NAME)-$(VERSION).tar.gz && sync && \
	cd $(NAME)-$(VERSION) && \
	./configure $(ARGS) && sync && \
	make && make install && sync && \
	cd ..

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Base/Libraries/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)
	for DOC in $(DOCS);do \
		cp -f $(NAME)-$(VERSION)/$$DOC $(PYRO_BINARY_DESTINATION)Base/Libraries/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)/; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Base/Libraries/$(NAME)-$(VERSION)/Documentation/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/plain"; \
		fi; \
	done;
	sync

update:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Base/Libraries/$(NAME)-$(VERSION)/`; do \
		if [ `file --brief --mime-type $$FILES | grep -c "text" -` != 0 ]; then \
			sed -i "s,$(PYRO_BINARY_DESTINATION)Base/Libraries/$(NAME)-$(VERSION),/boot,g" $$FILES; \
		fi; \
	done;
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Base/Libraries/$(NAME)-$(VERSION)/NIX/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

package:
	echo $(DESC) > $(TEMP)/zip_comment && \
	mkdir -p $(PYRO_BINARY_DESTINATION)Source/Base && \
	cp -f $(NAME)-$(VERSION).tar.gz $(PYRO_BINARY_DESTINATION)Source/Base/ && \
	cd $(PYRO_BINARY_DESTINATION)Base/Libraries/$(NAME)-$(VERSION) && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Base/Libraries/$(NAME)-$(VERSION).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Base/Libraries/$(NAME)-$(VERSION) && \
	sync

clean:
	rm -rf $(NAME)-$(VERSION)
	rm -f $(NAME)-$(VERSION).tar.gz
	sync
