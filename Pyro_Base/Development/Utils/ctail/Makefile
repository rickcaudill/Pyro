CFLAGS = -O3 -c

OBJS = main.o

ctail: $(OBJS)
	g++ $(OBJS) -o ctail

.cpp.o:
	g++ $(CFLAGS) $< -o $@

main.o: main.cpp

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: ctail inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Dev/Native/System/binary
	cp -f ctail $(PYRO_BINARY_DESTINATION)Dev/Native/System/binary/
	addattrib $(PYRO_BINARY_DESTINATION)Dev/Native/System/binary/ctail "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)Dev/Native/System/binary/ctail | cut -c1-32`
	sync

clean:
	rm -f ctail main.o
	sync
