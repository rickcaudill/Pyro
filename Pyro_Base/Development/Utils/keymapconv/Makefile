COPTS	= -c

OBJS	= keymapconv.o
EXE	= keymapconv

$(EXE) : $(OBJS)
	$(CXX) $(OBJS) -lpyro -o $@

.cpp.o:
	$(CXX) $(COPTS) $< -o $@

main.o : main.cpp
	$(CXX) $(COPTS) $< -o $@

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: $(EXE) inst clean

inst:
	strip --strip-all $(EXE)
	mkdir -p $(PYRO_BINARY_DESTINATION)Dev/Native/System/binary
	cp -f $(EXE) $(PYRO_BINARY_DESTINATION)Dev/Native/System/binary/
	addattrib $(PYRO_BINARY_DESTINATION)Dev/Native/System/binary/$(EXE) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)Dev/Native/System/binary/$(EXE) | cut -c1-32`
	sync

clean:
	rm -f $(OBJS)
	rm -f $(EXE)
	sync
