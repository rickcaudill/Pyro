# Makefile for ported software.

BACK=`pwd`
URL = http://www.cpan.org/src/5.0/
NAME = perl
VERSION = 5.10.0
ARGS =	-Dprefix=$(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)-$(VERSION)/NIX/$(NAME) \
		-des \
		-Dmksymlinks \
		-Accflags='-march=i586 -pipe' \
		-Aoptimize='-O2' \
		-Duseshrplib \
		-Dccflags='-DPERL_EXTERNAL_GLOB' \
		-de
DOCS = AUTHORS Changes Changes* Copying INSTALL README

dist: build doc update checksum package clean

build:
	wget -N $(URL)$(NAME)-$(VERSION).tar.gz && sync && \
	tar -xzkvf $(NAME)-$(VERSION).tar.gz && sync && \
	cd $(NAME)-$(VERSION) && \
	patch --dry-run	--strip=1 < ../PATCH/$(NAME).patch && \
	patch --strip=1 < ../PATCH/$(NAME).patch && sync && \
	cp -vrf ../PATCH/* ./ && sync && \
	./Configure $(ARGS) && sync && \
	make && make install && sync && \
	cd ..

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)
	for DOC in $(DOCS);do \
		cp -f $(NAME)-$(VERSION)/$$DOC $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)/; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)-$(VERSION)/Documentation/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/plain"; \
		fi; \
	done;
	sync

update:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)-$(VERSION)/NIX/`; do \
		if [ `file --brief --mime-type $$FILES | grep -c "text" -` != 0 ]; then \
			sed -i "s,$(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)-$(VERSION),/boot,g" $$FILES; \
		fi; \
	done;
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)-$(VERSION)/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

package:
	mkdir -p $(PYRO_BINARY_DESTINATION)Source/Extra && \
	cp -f $(NAME)-$(VERSION).tar.gz $(PYRO_BINARY_DESTINATION)Source/Extra/ && \
	cd $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)-$(VERSION) && \
	zip -ry9 $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)-$(VERSION).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)-$(VERSION) && \
	sync

clean:
	rm -rf $(NAME)-$(VERSION)
	rm -f $(NAME)-$(VERSION).tar.gz
	sync
