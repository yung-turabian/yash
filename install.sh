#!/bin/bash

PREFIX=/usr/local
BINDIR=$PREFIX/bin
MANDIR=$PREFIX/share/man/man1

mkdir -p $BINDIR
mkdir -p $MANDIR

cp hrry $BINDIR
chmod 755 $BINDIR/hrry

cp hrry.1 $MANDIR
gzip $MANDIR/hrry.1
