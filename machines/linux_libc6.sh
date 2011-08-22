#!/bin/sh

CC="gcc -O2 -g -W -Wall"
CFLAGS="-DCR_LINUX_LIBC6 -DCR_LINUX"
OBJDIR=bin.$OS_TYPE
XAW_LINK="-L/usr/X11R6/lib -lXaw -lXmu -lXext -lXt -lSM -lICE -lX11 -ldl -lelf"
MOTIF_LINK="-lXm -lXt -lSM -lICE -lX11"

export CC
export CFLAGS
export OBJDIR
export XAW_LINK
export MOTIF_LINK
