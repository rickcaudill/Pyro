# uname should report Pyro!!!

# Makefile for ported software.

BACK=`pwd`
URL = http://ftp.gnu.org/gnu/coreutils/
NAME = coreutils
VERSION = 6.4
ARGS =	--prefix=$(PYRO_BINARY_DESTINATION)Base/Utils/$(NAME)-$(VERSION)/NIX/$(NAME)
DESC = The GNU Core Utilities are the basic file, shell and text manipulation utilities.
DOCS = AUTHORS ChangeLog ChangeLog* COPYING INSTALL NEWS README THANKS THANKS-to-translators TODO

dist: build symlink doc update checksum package clean

build:
	wget -N $(URL)$(NAME)-$(VERSION).tar.gz && sync && \
	tar -xzkvf $(NAME)-$(VERSION).tar.gz && sync && \
	cd $(NAME)-$(VERSION) && \
	patch --dry-run	--strip=1 < ../PATCH/$(NAME).patch && \
	patch --strip=1 < ../PATCH/$(NAME).patch && sync && \
	./configure $(ARGS) && sync && \
	make && make install && sync && \
	cd ..

symlink:
	mkdir -p $(PYRO_BINARY_DESTINATION)Base/Utils/$(NAME)-$(VERSION)/System/nix/bin && \
	ln -sf /boot/NIX/coreutils/bin/ls $(PYRO_BINARY_DESTINATION)Base/Utils/$(NAME)-$(VERSION)/System/nix/bin/ls && sync && \
	sync

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Base/Utils/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)
	for DOC in $(DOCS);do \
		cp -f $(NAME)-$(VERSION)/$$DOC $(PYRO_BINARY_DESTINATION)Base/Utils/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)/; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Base/Utils/$(NAME)-$(VERSION)/Documentation/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/plain"; \
		fi; \
	done;
	sync

update:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Base/Utils/$(NAME)-$(VERSION)/NIX/`; do \
		if [ `file --brief --mime-type $$FILES | grep -c "text" -` != 0 ]; then \
			sed -i "s,$(PYRO_BINARY_DESTINATION)Base/Utils/$(NAME)-$(VERSION),/boot,g" $$FILES; \
		fi; \
	done;
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Base/Utils/$(NAME)-$(VERSION)/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

package:
	echo $(DESC) > $(TEMP)/zip_comment && \
	mkdir -p $(PYRO_BINARY_DESTINATION)Source/Base && \
	cp -f $(NAME)-$(VERSION).tar.gz $(PYRO_BINARY_DESTINATION)Source/Base/ && \
	cd $(PYRO_BINARY_DESTINATION)Base/Utils/$(NAME)-$(VERSION) && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Base/Utils/$(NAME)-$(VERSION).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Base/Utils/$(NAME)-$(VERSION) && \
	sync

clean:
	rm -rf $(NAME)-$(VERSION)
	rm -f $(NAME)-$(VERSION).tar.gz
	sync
