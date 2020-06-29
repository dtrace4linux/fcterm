#!/bin/sh

CC="gcc  -D_FORTIFY_SOURCE=0 -m32 -O2 -g -W -Wall -fno-stack-protector -fno-inline -fno-strict-aliasing"
CC="$CC -DHAVE_FREETYPE_XFT `pkgconf --cflags freetype2`"
CFLAGS="-DCR_LINUX_X86_32 -DCR_LINUX"
#FT=-lXft
OBJDIR=bin.$OS_TYPE
XAW_LINK="-L/usr/lib32 $FT -lXaw -lXmu -lXext -lXt -lSM -lICE -lX11 -ldl"
MOTIF_LINK="-lXm -lXt -lSM -lICE -lX11"

export CC
export CFLAGS
export OBJDIR
export XAW_LINK
export MOTIF_LINK
