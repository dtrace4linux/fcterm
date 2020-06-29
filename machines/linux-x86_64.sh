#!/bin/sh

CC=${CC:-gcc -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0 -g -W -Wall -fno-stack-protector -fno-inline -fno-strict-aliasing}
CC="$CC -DHAVE_FREETYPE_XFT `pkgconf --cflags freetype2`"
CFLAGS="-DCR_LINUX_X86_64 -DCR_LINUX"
#FT=-lXft
OBJDIR=${OBJDIR:-bin.$OS_TYPE}
XAW_LINK="$FT -lXaw -lXmu -lXext -lXt -lSM -lICE -lX11 -ldl"
MOTIF_LINK="-lXm -lXt -lSM -lICE -lX11 -ldl"

export CC
export CFLAGS
export OBJDIR
export XAW_LINK
export MOTIF_LINK
