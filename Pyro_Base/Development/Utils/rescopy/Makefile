CFLAGS   += -c
CXXFLAGS += -c

EXE = rescopy

OBJS = rescopy.o

OBJDIR	:= objs

include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) $(OBJDIR)/$(EXE)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/$(EXE): $(OBJS) Makefile
	$(CXX) $(OBJS) -o $(OBJDIR)/$(EXE) -lpyro

deps: $(OBJDIR) $(DEPS)

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Dev/Native/System/binary
	cp -f $(OBJDIR)/$(EXE) $(PYRO_BINARY_DESTINATION)Dev/Native/System/binary/
	addattrib $(PYRO_BINARY_DESTINATION)Dev/Native/System/binary/$(EXE) "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)Dev/Native/System/binary/$(EXE) | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
