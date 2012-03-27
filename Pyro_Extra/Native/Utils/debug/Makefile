COPTS	= -c

OBJS	= term.o
EXE	= dbterm

all: dbterm

$(EXE) : $(OBJS)
	$(CC) $(OBJS) -o $(EXE)

ftp:	ftp.o
	$(CC) ftp.o -o ftp

.cpp.o :
	$(CXX) $(COPTS) $< -o $@
.c.o :
	$(CC) $(COPTS) $< -o $@


term.o : term.c

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

BACK=`pwd`
DESC = Old AtheOS debug.

dist: all inst package clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Extra/Native/Utils/$(EXE)/System/binary/
	cp -f $(EXE) $(PYRO_BINARY_DESTINATION)Extra/Native/Utils/$(EXE)/System/binary/$(EXE)
	addattrib $(PYRO_BINARY_DESTINATION)Extra/Native/Utils/$(EXE)/System/binary/$(EXE) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)Extra/Native/Utils/$(EXE)/System/binary/$(EXE) | cut -c1-32`
	sync

package:
	echo $(DESC) > $(TEMP)/zip_comment && \
	cd $(PYRO_BINARY_DESTINATION)Extra/Native/Utils/$(EXE) && \
	zip -z < "$(TEMP)/zip_comment" -ry9 $(PYRO_BINARY_DESTINATION)Extra/Native/Utils/$(EXE).zip ./* && \
	cd $(BACK) && \
	rm -rf $(PYRO_BINARY_DESTINATION)Extra/Native/Utils/$(EXE) && \
	sync

clean:
	rm -f $(OBJS) dbterm rfs
	sync
