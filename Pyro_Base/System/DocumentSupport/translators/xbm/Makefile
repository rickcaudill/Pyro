OBJS = xbmtrans.o
OBJDIR = objs

#include $(PYRO_COMMON)/makefile.rules

all :	$(OBJDIR) $(OBJDIR)/xbmtrans.so

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/xbmtrans.so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared $(OBJS) -lpyro -o $(OBJDIR)/xbmtrans.so

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)/System/extensions/translators
	cp -f $(OBJDIR)/xbmtrans.so $(PYRO_BINARY_DESTINATION)/System/extensions/translators/
	addattrib $(PYRO_BINARY_DESTINATION)/System/extensions/translators/xbmtrans.so "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)/System/extensions/translators/xbmtrans.so | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	sync
