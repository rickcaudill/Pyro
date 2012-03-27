COPTS   = -I$(TOP)/src -c -Wall

OBJS = AddWindow.o AddLibrary.o ProjectPrefs.o GroupWindow.o ProjectWindow.o NewProject.o project.o main.o 
OBJDIR	:= objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

#
#	rules
#

$(OBJDIR)/%.o : %.c
	$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(CXX) $(COPTS) $< -o $@

all: objs sIDE

objs:
	mkdir objs

sIDE: $(OBJS)
	$(CXX) $(OBJS) -o sIDE -lpyro
	strip --strip-all sIDE
	rescopy sIDE -r ./images/*.png
	rescopy -a sIDE resources/sIDE.catalog
	for CATALOG in `ls -1 resources`;do \
		if [ -d "resources/$$CATALOG" ] ; then \
			echo $$CATALOG; \
			rescopy -a sIDE $$CATALOG/sIDE.catalog=resources/$$CATALOG/sIDE.catalog; \
		fi \
	done;
	addattrib sIDE os::Category Development

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: catalogs all templates inst clean

catalogs:
	cd resources; \
	catcomp -c sIDE.cd; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			echo $$CATALOG; \
			cd $$CATALOG; \
			catcomp -t sIDE.cd; \
			cd ..; \
		fi \
	done;
	sync

templates:
	build_templates.sh
	sync

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Dev/Native/Applications/Development/Templates
	cp -f sIDE $(PYRO_BINARY_DESTINATION)Dev/Native/Applications/Development/
	addattrib $(PYRO_BINARY_DESTINATION)Dev/Native/Applications/Development/sIDE "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)Dev/Native/Applications/Development/sIDE | cut -c1-32`
	cp -f Templates/*.zip $(PYRO_BINARY_DESTINATION)Dev/Native/Applications/Development/Templates/
	sync

clean: catalog-clean
	rm -rf $(OBJDIR) sIDE
	rm -f Templates/*.zip
	sync

catalog-clean:
	rm -f resources/sIDE.catalog resources/sIDE.h
	for CATALOG in `ls -1 resources`;do \
		rm -f resources/$$CATALOG/sIDE.catalog; \
	done;
	sync
