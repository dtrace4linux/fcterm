#!/bin/sh

CC="cc -O"
CFLAGS=-DCR_UNIXWARE21
OBJDIR=bin.$OS_TYPE
LD_RUN_PATH=/usr/X/lib
export LD_RUN_PATH
XAW_LINK="-L/usr/X/lib -lXaw -lXmu -lXext -lXt -lX11 ../../crisp/bin/foxlib.a -lsocket"
MOTIF_LINK="-lXm -lXt -lSM -lICE -lX11 -lsocket"

export CC
export CFLAGS
export OBJDIR
export XAW_LINK
export MOTIF_LINK
