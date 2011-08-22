#!/bin/sh

CC="gcc  -D_FORTIFY_SOURCE=0 -O2 -g -W -Wall"
CFLAGS="-DCR_LINUX_GLIBC23"
OBJDIR=bin.$OS_TYPE
XAW_LINK="-L/usr/X11R6/lib -lXaw -lXmu -lXext -lXt -lSM -lICE -lX11 -ldl -lelf"
MOTIF_LINK="-lXm -lXt -lSM -lICE -lX11"

export CC
export CFLAGS
export OBJDIR
export XAW_LINK
export MOTIF_LINK
