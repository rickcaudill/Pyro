# Makefile for ported software.

BACK=`pwd`
URL = ftp://ftp.porcupine.org/pub/security/
NAME = tcp_wrappers
VERSION = 7.6
ARGS =	REAL_DAEMON_DIR=$(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)-$(VERSION)/NIX/$(NAME) \
		generic
DOCS = BLURB CHANGES DISCLAIMER README README.NIS

dist: build doc update checksum package clean

build:
	wget --passive-ftp -N $(URL)$(NAME)_$(VERSION).tar.gz && sync && \
	tar -xzkvf $(NAME)_$(VERSION).tar.gz && sync && \
	cd $(NAME)_$(VERSION) && \
	make $(ARGS) && make install && sync && \
	cd ..

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)
	for DOC in $(DOCS);do \
		cp -f $(NAME)_$(VERSION)/$$DOC $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)-$(VERSION)/Documentation/nix/$(NAME)/; \
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
	cp -f $(NAME)_$(VERSION).tar.gz $(PYRO_BINARY_DESTINATION)Source/Extra/ && \
	cd $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)-$(VERSION) && \
	zip -ry9 $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)-$(VERSION).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)-$(VERSION) && \
	sync

clean:
	rm -rf $(NAME)_$(VERSION)
	rm -f $(NAME)_$(VERSION).tar.gz
	sync
