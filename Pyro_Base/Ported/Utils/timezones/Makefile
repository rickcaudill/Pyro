# Makefile for non-compilable items.

BACK=`pwd`
URL = ftp://ftp.iana.org/tz/
NAME = tz
FULLNAME = timezones
VERSION = latest
ARGS =	TOPDIR=$(PYRO_BINARY_DESTINATION)Base/Utils/$(FULLNAME)/NIX/$(FULLNAME)
DESC = Timezone database and utils.
DOCS = README Theory *.htm *.txt

dist: build doc checksum package clean

build:
	wget --passive-ftp --retr-symlinks -N $(URL)$(NAME)data-$(VERSION).tar.gz && sync && \
	wget --passive-ftp --retr-symlinks -N $(URL)$(NAME)code-$(VERSION).tar.gz && sync && \
	mkdir -p $(NAME)-$(VERSION) && \
	cd $(NAME)-$(VERSION) && \
	tar -xzkvf ../$(NAME)data-$(VERSION).tar.gz && sync && \
	tar -xzkvf ../$(NAME)code-$(VERSION).tar.gz && sync && \
	sed -i "s,CFLAGS=,CFLAGS = -DHAVE_SYMLINK=0,g" Makefile && \
	make $(ARGS) install && sync && \
	cd .. && \
	ln -sf /boot/NIX/$(FULLNAME)/etc/zoneinfo $(PYRO_BINARY_DESTINATION)Base/Utils/timezones/System/resources/timezones

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Base/Utils/$(FULLNAME)/Documentation/nix/$(FULLNAME)
	for DOC in $(DOCS);do \
		cp -f $(NAME)-$(VERSION)/$$DOC $(PYRO_BINARY_DESTINATION)Base/Utils/$(FULLNAME)/Documentation/nix/$(FULLNAME)/; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Base/Utils/$(FULLNAME)/Documentation/nix/$(FULLNAME)/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/plain"; \
		fi; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Base/Utils/$(FULLNAME)/Documentation/nix/$(FULLNAME)/ -name "*.htm"`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/html"; \
		fi; \
	done;
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Base/Utils/$(FULLNAME)/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

package:
	echo $(DESC) > $(TEMP)/zip_comment && \
	mkdir -p $(PYRO_BINARY_DESTINATION)Source/Base && \
	cp -f $(NAME)data-$(VERSION).tar.gz $(PYRO_BINARY_DESTINATION)Source/Base/ && \
	cp -f $(NAME)code-$(VERSION).tar.gz $(PYRO_BINARY_DESTINATION)Source/Base/ && \
	cd $(PYRO_BINARY_DESTINATION)Base/Utils/$(FULLNAME) && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Base/Utils/$(FULLNAME).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Base/Utils/$(FULLNAME) && \
	sync

clean:
	rm -rf $(NAME)-$(VERSION)
	rm -f $(NAME)data-$(VERSION).tar.gz
	rm -f $(NAME)code-$(VERSION).tar.gz
	sync
