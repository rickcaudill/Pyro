VERSION_MAJOR = 1
VERSION_MINOR = 0
LOBJS = objs/inneredit.o objs/codeview.o objs/format_c.o objs/format_java.o objs/format_perl.o objs/format_html.o objs/format_ruby.o
OBJS = objs/main.o
COPTS = -Wall -fPIC
EXE = testapp
LIB = libcodeview.so

all: $(LIB)

$(EXE): $(OBJS) $(LOBJS)
	$(CXX) $(OBJS) $(LOBJS) -lpyro -o $(EXE)

#$(EXE): $(OBJS) $(LIB)
#	$(CXX) $(OBJS) -lpyro -L. -lcodeview -o $(EXE)

$(LIB): $(LOBJS)
	$(CXX) $(LOBJS) -lpyro -shared -g -Wl,-soname,$(LIB).$(VERSION_MAJOR) -o $(LIB)

objs/%.o: %.cpp
	@mkdir -p objs
	$(CXX) $< $(COPTS) -c -o $@

objs:
	mkdir objs

objs/main.o: main.cpp codeview.h
objs/inneredit.o: inneredit.cpp inneredit.h format.h
objs/codeview.o:  codeview.cpp codeview.h inneredit.h
objs/format_c.o:  format_c.cpp format_c.h format.h
objs/format_perl.o:  format_perl.cpp format_perl.h format.h
objs/format_html.o:  format_html.cpp format_html.h format.h
objs/format_java.o:  format_c.cpp format_c.h format_java.cpp \
	format_java.h format.h
objs/format_ruby: format_ruby.cpp format_ruby.h format.h

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

DOCS = CHANGES COPYING README TODO

dist: all inst doc-general doc headers checksum clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Dev/Native/System/libraries
	mv -f $(LIB) $(LIB).$(VERSION_MAJOR).$(VERSION_MINOR)
	ln -fs $(LIB).$(VERSION_MAJOR).$(VERSION_MINOR) $(LIB).$(VERSION_MAJOR)
	ln -fs $(LIB).$(VERSION_MAJOR) $(LIB)
	cp -f $(LIB) $(PYRO_BINARY_DESTINATION)Dev/Native/System/libraries/
	cp -f $(LIB).$(VERSION_MAJOR) $(PYRO_BINARY_DESTINATION)Dev/Native/System/libraries/
	cp -f $(LIB).$(VERSION_MAJOR).$(VERSION_MINOR) $(PYRO_BINARY_DESTINATION)Dev/Native/System/libraries/
	sync

doc-general:
	mkdir -p $(PYRO_BINARY_DESTINATION)Dev/Native/Documentation/Development/codeview
	for DOC in $(DOCS);do \
		cp -f ./$$DOC $(PYRO_BINARY_DESTINATION)Dev/Native/Documentation/Development/codeview/; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Dev/Native/Documentation/Development/codeview/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/plain"; \
		fi; \
	done;
	sync

doc:
	mkdir -p docs
	doxytag -t docs/atheos.gui.tag /boot/Documentation/API/CodeView/
	doxygen doxygen.conf
	rm -f docs/atheos.gui.tag
	rm -f docs/installdox
	mkdir -p $(PYRO_BINARY_DESTINATION)Dev/Native/Documentation/API/CodeView
	cp -rf docs/* $(PYRO_BINARY_DESTINATION)Dev/Native/Documentation/API/CodeView/
	sync

headers:
	mkdir -p $(PYRO_BINARY_DESTINATION)Dev/Native/Include/codeview
	cp -f codeview.h $(PYRO_BINARY_DESTINATION)Dev/Native/Include/codeview/
	cp -f format.h $(PYRO_BINARY_DESTINATION)Dev/Native/Include/codeview/
	cp -f format_c.h $(PYRO_BINARY_DESTINATION)Dev/Native/Include/codeview/
	cp -f format_java.h $(PYRO_BINARY_DESTINATION)Dev/Native/Include/codeview/
	cp -f format_perl.h $(PYRO_BINARY_DESTINATION)Dev/Native/Include/codeview/
	cp -f format_html.h $(PYRO_BINARY_DESTINATION)Dev/Native/Include/codeview/
	cp -f format_ruby.h $(PYRO_BINARY_DESTINATION)Dev/Native/Include/codeview/
	for FILES in `find $(PYRO_BINARY_DESTINATION)Dev/Native/Include/codeview/ -name "*.h"`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "os::MimeType" "text/x-h"; \
		fi; \
	done;
	sync

checksum:
	for FILES in `find $(PYRO_BINARY_DESTINATION)Dev/Native/Include/codeview/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Dev/Native/Documentation/Development/codeview/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	for FILES in `find $(PYRO_BINARY_DESTINATION)Dev/Native/System/libraries/`; do \
		if test -f $$FILES; then \
			addattrib $$FILES "CheckSum::MD5" `md5sum $$FILES | cut -c1-32`; \
		fi; \
	done;
	sync

clean:
	rm -f $(OBJS)
	rm -f $(LOBJS)
	rm -f $(EXE)
	rm -f $(LIB)
	rm -f $(LIB).$(VERSION_MAJOR)
	rm -f $(LIB).$(VERSION_MAJOR).$(VERSION_MINOR)
	rm -rf objs
	rm -rf docs
	sync
