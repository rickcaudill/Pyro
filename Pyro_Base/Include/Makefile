# Makefile for non-compilable items.

dist: copy mimetypes checksum

copy:
	mkdir -p $(PYRO_BINARY_DESTINATION)Dev/Native/Include
	cp -rf * $(PYRO_BINARY_DESTINATION)Dev/Native/Include/
	rm -f $(PYRO_BINARY_DESTINATION)Dev/Native/Include/Makefile
	sync

mimetypes:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Dev/Native/Include/ -name "*.h"`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/x-h"; \
		fi; \
	done;
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Dev/Native/Include/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

clean:
	sync
