# Makefile for ported software.

BACK=`pwd`
URL = http://mirrors.dotsrc.org/apache/apr/
NAME = apr
VERSION = 1.4.5
ARGS =	--prefix=$(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)-$(VERSION)/NIX/$(NAME)
DOCS = CHANGES LICENSE NOTICE README

dist: build doc update checksum package clean

build:
	wget -N $(URL)$(NAME)-$(VERSION).tar.gz && sync && \
	tar -xzkvf $(NAME)-$(VERSION).tar.gz && sync && \
	cd $(NAME)-$(VERSION) && \
	patch --dry-run	--strip=1 < ../PATCH/$(NAME).patch && \
	patch --strip=1 < ../PATCH/$(NAME).patch && sync && \
	./configure $(ARGS) && sync && \
	make && make install && sync && \
	cd ..

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)
	for DOC in $(DOCS);do \
		cp -f $(NAME)-$(VERSION)/$$DOC $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)/; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)-$(VERSION)/Documentation/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/plain"; \
		fi; \
	done;
	sync

update:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)-$(VERSION)/NIX/`; do \
		if [ `file --brief --mime-type $$FILES | grep -c "text" -` != 0 ]; then \
			sed -i "s,$(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)-$(VERSION),/boot,g" $$FILES; \
		fi; \
	done;
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)-$(VERSION)/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

package:
	mkdir -p $(PYRO_BINARY_DESTINATION)Source/Extra && \
	cp -f $(NAME)-$(VERSION).tar.gz $(PYRO_BINARY_DESTINATION)Source/Extra/ && \
	cd $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)-$(VERSION) && \
	zip -ry9 $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)-$(VERSION).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)-$(VERSION) && \
	sync

clean:
	rm -rf $(NAME)-$(VERSION)
	rm -f $(NAME)-$(VERSION).tar.gz
	sync
