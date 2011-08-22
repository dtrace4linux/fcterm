#!/bin/sh

CC="gcc -O -I$OPENWINHOME/include"
CFLAGS=-DCR_SOLARIS2_SPARC
OBJDIR=bin.$OS_TYPE
XAW_LINK="-L$OPENWINHOME/lib -R$OPENWINHOME/lib -lXaw -lXmu -lXext -lXt -lX11 -lsocket"
MOTIF_LINK="-lXm -lXt -lX11 -lsocket"

export CC
export CFLAGS
export OBJDIR
export XAW_LINK
export MOTIF_LINK
