# Makefile for non-compilable items.

dist: copy mimetypes checksum

copy:
	mkdir -p $(PYRO_BINARY_DESTINATION)/Documentation
	cp -r * $(PYRO_BINARY_DESTINATION)/Documentation/
	rm -f $(PYRO_BINARY_DESTINATION)/Documentation/Makefile
	sync

mimetypes:
	for FILES in `find $(PYRO_BINARY_DESTINATION)/Documentation/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/plain"; \
		fi; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)/Documentation/ -name "*.txt"`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/plain"; \
		fi; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)/Documentation/ -name "*.html"`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/html"; \
		fi; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)/Documentation/ -name "*.png"`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "image/png"; \
		fi; \
	done;
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)/Documentation/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

clean:
	sync
