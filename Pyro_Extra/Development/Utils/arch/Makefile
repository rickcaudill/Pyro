# Makefile for ported software.

BACK=`pwd`
URL = http://ftp.gnu.org/gnu/gnu-arch/
NAME = arch
VERSION = 1.3.5
ARGS =	--prefix=$(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)-$(VERSION)/NIX/$(NAME)
DOCS = INSTALL index.txt src/COPYING src/ChangeLog

dist: build doc update checksum package clean

build:
	wget -N $(URL)tla-$(VERSION).tar.gz && sync && \
	tar -xzkvf tla-$(VERSION).tar.gz && sync && \
	cd tla-$(VERSION) && \
	./src/configure $(ARGS) && sync && \
	make && make install && sync && \
	cd ..

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)
	for DOC in $(DOCS);do \
		cp -f tla-$(VERSION)/$$DOC $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)/; \
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
	cp -f tla-$(VERSION).tar.gz $(PYRO_BINARY_DESTINATION)Source/Extra/$(NAME)-$(VERSION).tar.gz && \
	cd $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)-$(VERSION) && \
	zip -ry9 $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)-$(VERSION).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)-$(VERSION) && \
	sync

clean:
	rm -rf tla-$(VERSION)
	rm -f tla-$(VERSION).tar.gz
	sync
