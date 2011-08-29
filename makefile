# Pretty basic makefile. Please hack to your hearts content.
#
# Type 'make' to build an athena widgets version.
# Type 'make motif' to build a motif version.
#
# Specify where to get include files.
CRISP=../../crisp
BIN=bin.linux-x86_32
XPM_LIB=$(CRISP)/$(BIN)/libXpm.a $(CRISP)/$(BIN)/foxlib.a $(CRISP)/$(BIN)/x11.obj/w_draw.o
XPM_INCLUDE=$(CRISP)/x11/xpm
INC=-I. -Iinclude -I$(XPM_INCLUDE) -I$(CRISP)/foxlib -I$(CRISP)/include -I$(CRISP)/x11/widgets

LINK=$(PURIFY) $(CC) $(PROF)

#OBJDIR=	obj
H	= include/code.h include/sequence.h include/fcterm.h include/ctw.h
OBJ=	\
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

motif:
	$(MAKE) PROTO="-D_NO_PROTO" LIB="-L/usr/dt/lib -R/usr/dt/lib -L/usr/openwin/lib -R/usr/openwin/lib -L../../lib -lXpm -lXmu -lXm -lXt -lX11" motif1

motif1:	$(OBJDIR)/fmcterm.o $(OBJDIR)/motif.o $(OBJ)
	$(LINK) $(PROF) -o $(OBJDIR)/fmcterm $(OBJDIR)/fmcterm.o $(OBJDIR)/motif.o $(OBJ) $(LIB)

$(OBJDIR)/ctw.o:		ctw.c include/ctw.h include/ctwP.h include/sequence.h include/code.h keyboard.c
	$(CC) $(INC) $(CFLAGS) -c -o $(OBJDIR)/ctw.o ctw.c
$(OBJDIR)/fcterm.o:	fcterm.c $(H) fcterm.bitmap include/scrbar.h include/build.h
	$(CC) $(INC) $(CFLAGS) -c -o $(OBJDIR)/fcterm.o fcterm.c
$(OBJDIR)/fmcterm.o:	fmcterm.c $(H) fcterm.bitmap include/scrbar.h
	$(CC) $(INC) -I/usr/dt/include  $(CFLAGS) -c -o $(OBJDIR)/fmcterm.o fmcterm.c
$(OBJDIR)/term.o:		term.c $(H) fcterm.bitmap
	$(CC) $(INC) $(CFLAGS) -c -o $(OBJDIR)/term.o term.c
$(OBJDIR)/motif.o:	motif.c motif.h
	$(CC) $(INC) -I/usr/dt/include $(CFLAGS) -c -o $(OBJDIR)/motif.o motif.c
$(OBJDIR)/rotate.o:	rotate.c
	$(CC) $(INC) $(CFLAGS) -c -o $(OBJDIR)/rotate.o rotate.c
$(OBJDIR)/scrbar.o:	scrbar.c include/scrbar.h include/scrbarP.h
	$(CC) $(INC) $(CFLAGS) -c -o $(OBJDIR)/scrbar.o scrbar.c

$(OBJDIR)/pty: pty.c ptysrv.h
	$(CC) $(INC) -I$(CRISP)/include $(CFLAGS) -o $(OBJDIR)/pty pty.c $(CRISP)/$(BIN)/foxlib.a
$(OBJDIR)/ptysrv: ptysrv.c ptysrv.h
	$(CC) -g -fno-inline $(INC) -I$(CRISP)/include $(CFLAGS) -o $(OBJDIR)/ptysrv ptysrv.c $(CRISP)/$(BIN)/foxlib.a -ldl

newf:
	tar cvf - `find . -type f -newer TIMESTAMP ! -name tags ! -name y.tab.c ! -name config.def | grep -v /bin ` | gzip -9 > $(HOME)/tmp/src.ctw.tar.gz

release:
	mkrelease.pl bin/fcterm samples fccat

release2:
	git commit . || exit 1
	strip bin/fcterm
	elfrewrite bin/fcterm
	build=b`grep build_no include/build.h | sed -e 's/^.* \([0-9][0-9]*\).*$$/\1/'` ; \
	cd .. ; mv ctw ctw-$$build ; \
	tar cvf - ctw-$$build/bin/fcterm ctw-$$build/README* ctw-$$build/Changes ctw-$$build/samples ctw-$$build/fccat | gzip -9 > /tmp/fcterm-$$build.tar.gz ; \
	mv ctw-$$build ctw ; \
	scp /tmp/fcterm-$$build.tar.gz minny:release/website/tools ; \
	ssh minny rm -f release/website/tools/fcterm-current.tar.gz ; \
	ssh minny ln -s fcterm-$$build.tar.gz release/website/tools/fcterm-current.tar.gz

