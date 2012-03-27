# Makefile for ported software.

BACK=`pwd`
URL = http://tukaani.org/fakedestdir/
NAME = fakedestdir

dist: build inst doc checksum package clean

build:
	mkdir $(NAME) && \
	cd $(NAME) && \
	wget -N $(URL)$(NAME) && sync && \
	wget -N $(URL) && sync && \
	chmod +x $(NAME) && \
	mv index.html $(NAME).html
	cd ..

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)/NIX/$(NAME)/bin
	cp -f $(NAME)/$(NAME) $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)/NIX/$(NAME)/bin/

doc:
	mkdir -p $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)/Documentation/nix/$(NAME)
	cp -f $(NAME)/$(NAME).html $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)/Documentation/nix/$(NAME)/; \
	addattrib $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)/Documentation/nix/$(NAME)/$(NAME).html "os::MimeType" "text/html"; \

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME)/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

package:
	cd $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME) && \
	zip -ry9 $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Extra/Ported/Utils/$(NAME) && \
	sync

clean:
	rm -rf $(NAME)
	sync
