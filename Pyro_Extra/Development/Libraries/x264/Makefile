# Makefile for ported software.

BACK=`pwd`
URL = ftp://ftp.videolan.org/pub/x264/snapshots/
NAME = x264
ARGS =	--prefix=$(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)/NIX/$(NAME) \
		--enable-pic \
		--enable-static \
		--disable-asm
DOCS = AUTHORS COPYING

dist: build doc update checksum package clean

build:
	wget --passive-ftp --retr-symlinks -N $(URL)last_stable_x264.tar.bz2 && sync && \
	tar -xjkvf last_stable_x264.tar.bz2 && sync && \
	cd $(NAME)* && \
	patch --dry-run	--strip=1 < ../PATCH/$(NAME).patch && \
	patch --strip=1 < ../PATCH/$(NAME).patch && sync && \
	./configure $(ARGS) && sync && \
	make && make install && sync && \
	cd ..

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)/Documentation/nix/$(NAME)
	for DOC in $(DOCS);do \
		cp -f $(NAME)*/$$DOC $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)/Documentation/nix/$(NAME)/; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)/Documentation/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/plain"; \
		fi; \
	done;
	sync

update:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)/NIX/`; do \
		if [ `file --brief --mime-type $$FILES | grep -c "text" -` != 0 ]; then \
			sed -i "s,$(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME),/boot,g" $$FILES; \
		fi; \
	done;
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME)/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

package:
	mkdir -p $(PYRO_BINARY_DESTINATION)Source/Extra && \
	cp -f last_stable_x264.tar.bz2 $(PYRO_BINARY_DESTINATION)Source/Extra/$(NAME).tar.bz2 && \
	cd $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME) && \
	zip -ry9 $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Extra/Ported/Libraries/$(NAME) && \
	sync

clean:
	rm -rf $(NAME)*
	rm -f last_stable_x264.tar.bz2
	sync
