# Makefile for ported software.

BACK=`pwd`
URL = ftp://ftp.software.ibm.com/software/globalization/
NAME = icu
VERSION = 3.6
MAJORVERSION = 3
MINORVERSION = 6
ARGS =	--prefix=$(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)-$(VERSION)/NIX/$(NAME) \
		--disable-threads \
		--enable-static \
		--disable-shared \
		--disable-64bit-libs
DOCS = *.html

dist: build doc update checksum package clean

build:
	wget --passive-ftp -N $(URL)$(NAME)/$(VERSION)/$(NAME)4c-$(MAJORVERSION)_$(MINORVERSION)-src.tgz && sync && \
	tar -xzkvf $(NAME)4c-$(MAJORVERSION)_$(MINORVERSION)-src.tgz && sync && \
	cd $(NAME) && \
	cp -vrf ../PATCH/* ./ && sync && \
	cd source && \
	./configure $(ARGS) && sync && \
	make && make install && sync && \
	cd ..

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)
	for DOC in $(DOCS);do \
		cp -f $(NAME)/$$DOC $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)/; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)-$(VERSION)/Documentation/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/html"; \
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
	cp -f $(NAME)4c-$(MAJORVERSION)_$(MINORVERSION)-src.tgz $(PYRO_BINARY_DESTINATION)Source/Extra/ && \
	cd $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)-$(VERSION) && \
	zip -ry9 $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)-$(VERSION).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)-$(VERSION) && \
	sync

clean:
	rm -rf $(NAME)
	rm -f $(NAME)4c-$(MAJORVERSION)_$(MINORVERSION)-src.tgz
	sync
