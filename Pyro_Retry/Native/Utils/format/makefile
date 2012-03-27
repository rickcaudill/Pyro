#Makefile for format
COPTS = -Wall -c -O2 -fexceptions

APPBIN = /Applications/format
OBJS = main.o application.o mainwindow.o splash.o

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

all : translations objs "format"

$(OBJDIR)/mainwindow.o: mainwindowLayout.cpp

translations:

objs:
	@mkdir -p objs

"format": $(OBJS)
	@echo Linking...
	@$(CXX) $(OBJS) -o "format"  -lpyro
	@echo Adding resources...
	@rescopy "format" -r images/icon48x48.png images/logo.png 
	@addattrib "format" os::Category "System Tools"

clean:
	@echo Cleaning...
	@rm -f $(OBJDIR)/*
	@rm -f "format"

deps:

install: all
	@echo Installing...
	@mkdir -p $(IMAGE)/Applications/format
	@cp "format" "$(IMAGE)/Applications/format/format"

dist: install
