COPTS = -c

OBJS = main.o gui/clockview.o
EXE	= aclock

$(EXE) : $(OBJS)
	$(CXX) $(OBJS) -lpyro -lm -o $@
	rescopy -r $(EXE) icon48x48.png 
	rescopy -a $(EXE) resources/$(EXE).catalog
	for CATALOG in `ls -1 resources`;do \
		if [ -d "resources/$$CATALOG" ] ; then \
			echo $$CATALOG; \
			rescopy -a $(EXE) $$CATALOG/$(EXE).catalog=resources/$$CATALOG/$(EXE).catalog; \
		fi \
	done;
	addattrib $(EXE) os::Category "Other"

#.c.o :
#	$(CC) $(COPTS) $< -o $@
.cpp.o:
	$(CXX) $(COPTS) $< -o $@

main.o:	main.cpp gui/clockview.h mywindow.h
gui/clockview.o: gui/clockview.cpp gui/clockview.h

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

BACK=`pwd`
DESC = Analog/Digital clock.

dist: catalogs $(EXE) inst package clean

catalogs:
	cd resources; \
	catcomp -c $(EXE).cd; \
	for CATALOG in `ls -1`;do \
		if [ -d "$$CATALOG" ] ; then \
			echo $$CATALOG; \
			cd $$CATALOG; \
			catcomp -t $(EXE).cd; \
			cd ..; \
		fi \
	done;
	sync

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Extra/Native/Utils/$(EXE)/System/binary/
	cp -f $(EXE) $(PYRO_BINARY_DESTINATION)Extra/Native/Utils/$(EXE)/System/binary/$(EXE)
	strip --strip-all $(PYRO_BINARY_DESTINATION)Extra/Native/Utils/$(EXE)/System/binary/$(EXE)
	addattrib $(PYRO_BINARY_DESTINATION)Extra/Native/Utils/$(EXE)/System/binary/$(EXE) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)Extra/Native/Utils/$(EXE)/System/binary/$(EXE) | cut -c1-32`
	sync

package:
	echo $(DESC) > $(TEMP)/zip_comment && \
	cd $(PYRO_BINARY_DESTINATION)Extra/Native/Utils/$(EXE) && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Extra/Native/Utils/$(EXE).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Extra/Native/Utils/$(EXE) && \
	sync

clean: catalog-clean
	rm -f $(OBJS)
	rm -f $(EXE)
	sync

catalog-clean:
	rm -f resources/$(EXE).catalog resources/$(EXE).h
	for CATALOG in `ls -1 resources`;do \
		rm -f resources/$$CATALOG/$(EXE).catalog; \
	done;
	sync
