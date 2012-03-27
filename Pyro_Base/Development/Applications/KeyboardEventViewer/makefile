# Makefile for KeyboardEventViewer

COPTS = -Wall -c -O2 -fexceptions

OBJS = main.o application.o mainwindow.o view.o common.o

OBJDIR := objs
OBJS	:= $(addprefix $(OBJDIR)/,$(OBJS))

# Rules
$(OBJDIR)/%.o : %.c
	@echo Compiling : $<
	@$(CC) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	@echo Compiling : $<
	@$(CXX) $(COPTS) $< -o $@

$(OBJDIR)/%.o : %.s
	@echo Assembling : $<
	@$(CC) $(COPTS) -x assembler-with-cpp $< -o $@

all : translations objs "KeyboardEventViewer"

translations:

objs:
	@mkdir -p objs

"KeyboardEventViewer": $(OBJS)
	@echo Linking...
	@$(CXX) $(OBJS) -o "KeyboardEventViewer" -lpyro
	@echo Adding resources...
	@rescopy "KeyboardEventViewer" -r icon48x48.png 
	@addattrib "KeyboardEventViewer" os::Category Development

# ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

dist: all inst clean

inst:
	mkdir -p $(PYRO_BINARY_DESTINATION)Dev/Native/Applications/Development
	cp -f KeyboardEventViewer $(PYRO_BINARY_DESTINATION)Dev/Native/Applications/Development/
	addattrib $(PYRO_BINARY_DESTINATION)Dev/Native/Applications/Development/KeyboardEventViewer "CheckSum::MD5" `md5sum $(PYRO_BINARY_DESTINATION)Dev/Native/Applications/Development/KeyboardEventViewer | cut -c1-32`
	sync

clean:
	rm -rf $(OBJDIR)
	rm -f "KeyboardEventViewer"
	sync
