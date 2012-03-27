# Makefile for ported software.

BACK=`pwd`
URL = http://www.cmake.org/files/
NAME = cmake
VERSION = 2.8
RELEASE = 7
ARGS =	--prefix=$(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)-$(VERSION).$(RELEASE)/NIX/$(NAME)
DOCS = *.txt

dist: build doc update checksum package clean

build:
	wget -N $(URL)v$(VERSION)/$(NAME)-$(VERSION).$(RELEASE).tar.gz && sync && \
	tar -xzkvf $(NAME)-$(VERSION).$(RELEASE).tar.gz && sync && \
	cd $(NAME)-$(VERSION).$(RELEASE) && \
#	export DLL_PATH=$DLL_PATH:\@bindir\@/../lib && \
	./configure $(ARGS) && sync && \
	make && make install && sync && \
	cd ..

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)-$(VERSION).$(RELEASE)/Documentation/nix/$(NAME)
	for DOC in $(DOCS);do \
		cp -f $(NAME)-$(VERSION).$(RELEASE)/$$DOC $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)-$(VERSION).$(RELEASE)/Documentation/nix/$(NAME)/; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)-$(VERSION).$(RELEASE)/Documentation/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/plain"; \
		fi; \
	done;
	sync

update:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)-$(VERSION).$(RELEASE)/NIX/`; do \
		if [ `file --brief --mime-type $$FILES | grep -c "text" -` != 0 ]; then \
			sed -i "s,$(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)-$(VERSION).$(RELEASE),/boot,g" $$FILES; \
		fi; \
	done;
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)-$(VERSION).$(RELEASE)/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

package:
	mkdir -p $(PYRO_BINARY_DESTINATION)Source/Extra && \
	cp -f $(NAME)-$(VERSION).$(RELEASE).tar.gz $(PYRO_BINARY_DESTINATION)Source/Extra/ && \
	cd $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)-$(VERSION).$(RELEASE) && \
	zip -ry9 $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)-$(VERSION).$(RELEASE).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)-$(VERSION).$(RELEASE) && \
	sync

clean:
	rm -rf $(NAME)-$(VERSION).$(RELEASE)
	rm -f $(NAME)-$(VERSION).$(RELEASE).tar.gz
	sync
