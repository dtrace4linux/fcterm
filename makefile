#! /bin/make
#
# Pretty basic makefile. Please hack to your hearts content.
#
# Type 'make' to build an athena widgets version.
# Type 'make motif' to build a motif version.
#
# Specify where to get include files.
CRISP=../../crisp
BIN=bin.linux-x86_64
XPM_LIB=$(CRISP)/$(BIN)/libXpm.a $(CRISP)/$(BIN)/foxlib.a $(CRISP)/$(BIN)/x11.obj/w_draw.o
XPM_INCLUDE=$(CRISP)/x11/xpm
INC=-I. -Iinclude -I$(XPM_INCLUDE) -I$(CRISP)/foxlib -I$(CRISP)/include -I$(CRISP)/x11/widgets

LINK=$(PURIFY) $(CC) $(PROF)

#OBJDIR=	obj
H	= include/code.h include/sequence.h include/fcterm.h include/ctw.h
OBJ=	\
	$(OBJDIR)/group.o \
	$(OBJDIR)/term.o \
	$(OBJDIR)/ctw.o \
	$(OBJDIR)/rotate.o \
	$(OBJDIR)/scrbar.o

all:
	. ./config.def ; $(MAKE) all0

all0: $(OBJDIR)/fcterm $(OBJDIR)/pty $(OBJDIR)/ptysrv

include/sequence.h: mkseq.sh
	mkseq.sh > include/sequence.h
include/code.h: mkcode.sh
	mkcode.sh > include/code.h

$(OBJDIR)/fcterm:	$(OBJDIR)/fcterm.o $(OBJ) 
	$(PURIFY) $(CC) -o $(OBJDIR)/fcterm $(OBJDIR)/fcterm.o $(OBJ) $(XPM_LIB) $(XAW_LINK)

install:
	rm -f $(HOME)/bin/fcterm
	cp bin.linux-x86_64/fcterm $(HOME)/bin/fcterm-`bin/fcterm -version`
	ln -s fcterm-`bin/fcterm -version` $(HOME)/bin/fcterm

make_macos:
	export PLATFORM=apple-osx-10.7-x86 ; \
	if [ ! -d ../../crisp/bin.$$PLATFORM ]; then \
		export PLATFORM=apple-osx-10.6-x86 ; \
	fi ; \
	if [ ! -d bin.$PLATFORM ]; then \
		mkdir bin.$$PLATFORM 2>/dev/null ; \
	fi ; \
	OS_TYPE=$$PLATFORM BIN=bin.$$PLATFORM OBJDIR=bin.$$PLATFORM \
	XPM_LIB="$(CRISP)/bin.$$PLATFORM/libXpm.a $(CRISP)/bin.$$PLATFORM/foxlib.a $(CRISP)/bin.$$PLATFORM/x11.obj/w_draw.o " \
	XAW_LINK="-L/usr/X11/lib -lXaw -lXmu -lXt -lSM -lX11" \
	CFLAGS="-DCR_APPLE_OSX_10_7_X86 " \
	$(MAKE) -e all0
make_rpi:
	-mkdir bin.linux-armv6l
	-ln -s bin.linux-armv6l bin
	$(MAKE) BIN=bin.linux-armv6l \
		CFLAGS="-DCR_LINUX_ARMV6L" \
		OS_TYPE=linux-armv6l \
		OBJDIR=bin.linux-armv6l \
		XAW_LINK="$(FT) -lXaw -lXmu -lXext -lXt -lSM -lICE -lX11 -ldl" \
		all0

motif:
	$(MAKE) PROTO="-D_NO_PROTO" LIB="-L/usr/dt/lib -R/usr/dt/lib -L/usr/openwin/lib -R/usr/openwin/lib -L../../lib -lXpm -lXmu -lXm -lXt -lX11" motif1

motif1:	$(OBJDIR)/fmcterm.o $(OBJDIR)/motif.o $(OBJ)
	$(LINK) $(PROF) -o $(OBJDIR)/fmcterm $(OBJDIR)/fmcterm.o $(OBJDIR)/motif.o $(OBJ) $(LIB)

$(OBJDIR)/ctw.o:		src/ctw.c include/ctw.h include/ctwP.h include/sequence.h include/code.h src/keyboard.c
	$(CC) $(INC) $(CFLAGS) -c -o $(OBJDIR)/ctw.o src/ctw.c
$(OBJDIR)/fcterm.o:	src/fcterm.c $(H) fcterm.bitmap include/scrbar.h include/build.h
	$(CC) $(INC) $(CFLAGS) -c -o $(OBJDIR)/fcterm.o src/fcterm.c
$(OBJDIR)/fmcterm.o:	fmcterm.c $(H) fcterm.bitmap include/scrbar.h
	$(CC) $(INC) -I/usr/dt/include  $(CFLAGS) -c -o $(OBJDIR)/fmcterm.o fmcterm.c
$(OBJDIR)/group.o:	src/group.c
	$(CC) $(INC) $(CFLAGS) -c -o $(OBJDIR)/group.o src/group.c
$(OBJDIR)/term.o:		src/term.c $(H) fcterm.bitmap
	$(CC) $(INC) $(CFLAGS) -c -o $(OBJDIR)/term.o src/term.c
$(OBJDIR)/motif.o:	motif.c motif.h
	$(CC) $(INC) -I/usr/dt/include $(CFLAGS) -c -o $(OBJDIR)/motif.o motif.c
$(OBJDIR)/rotate.o:	src/rotate.c
	$(CC) $(INC) $(CFLAGS) -c -o $(OBJDIR)/rotate.o src/rotate.c
$(OBJDIR)/scrbar.o:	src/scrbar.c include/scrbar.h include/scrbarP.h
	$(CC) $(INC) $(CFLAGS) -c -o $(OBJDIR)/scrbar.o src/scrbar.c

$(OBJDIR)/pty: src/pty.c include/ptysrv.h
	$(CC) $(INC) -I$(CRISP)/include $(CFLAGS) -o $(OBJDIR)/pty src/pty.c $(CRISP)/$(BIN)/foxlib.a -ldl
$(OBJDIR)/ptysrv: src/ptysrv.c include/ptysrv.h
	$(CC) -g -fno-inline $(INC) -I$(CRISP)/include $(CFLAGS) -o $(OBJDIR)/ptysrv src/ptysrv.c $(CRISP)/$(BIN)/foxlib.a -ldl

mac: 
	gcc -DCOCOA -ObjC -Iinclude -I$(CRISP)/include -I$(CRISP)/foxlib \
		-o main macos/main.m bin/term.o -framework AppKit

newf:
	tar cvf - `find . -type f -newer TIMESTAMP ! -name tags ! -name y.tab.c ! -name config.def | grep -v /bin ` | gzip -9 > $(HOME)/tmp/src.ctw.tar.gz

release:
	echo mkrelease.pl -arch bin/fcterm samples fccat
	mkrelease.pl bin/fcterm samples fccat
noise:
	v=ctw-`bin/fcterm -version` ; \
	cd .. ; mv ctw $$v ; \
	tar cvf - $$v/bin/fcterm $$v/samples $$v/fccat | bzip2 >/tmp/noise2.tar.bz2 ; \
	mv $$v ctw
	gfx -o /tmp/noise2.bmp /tmp/noise2.tar.bz2
	cd /tmp ; (echo cd docroot/images ; echo put noise2.bmp ) | ftp homepages.demon.co.uk


release2:
	git commit . || exit 1
	strip bin/fcterm
	elfrewrite bin/fcterm
	build=b`grep build_no include/build.h | sed -e 's/^.* \([0-9][0-9]*\).*$$/\1/'` ; \
	cd .. ; mv ctw ctw-$$build ; \
	tar cvf - ctw-$$build/bin/fcterm ctw-$$build/README* ctw-$$build/Changes ctw-$$build/samples ctw-$$build/fccat | gzip -9 > /tmp/fcterm-$$build.tar.gz ; \
	mv ctw-$$build ctw ; \
	scp /tmp/fcterm-$$build.tar.gz crispgw:release/website/tools ; \
	ssh crispgw rm -f release/website/tools/fcterm-current.tar.gz ; \
	ssh crispgw ln -s fcterm-$$build.tar.gz release/website/tools/fcterm-current.tar.gz

