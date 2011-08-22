#!/bin/sh

CC="cc -O"
CFLAGS=-DCR_UNIXWARE7
OBJDIR=bin.$OS_TYPE
XAW_LINK="-lXaw -lXmu -lXext -lXt -lSM -lICE -lX11 -lsocket"
MOTIF_LINK="-lXm -lXt -lSM -lICE -lX11 -lsocket"

export CC
export CFLAGS
export OBJDIR
export XAW_LINK
export MOTIF_LINK
