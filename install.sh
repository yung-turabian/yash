#!/bin/bash

PREFIX=/usr
BINDIR=$PREFIX/bin
SYSCONFDIR=$PREFIX/etc
USRCONFDIR=~/.config
MANDIR=$PREFIX/share/man/man1

mkdir -p $BINDIR
mkdir -p $MANDIR

cp bin/hrry $BINDIR
chmod 755 $BINDIR/hrry

cp share/man/man1/hrry.1 $MANDIR
gzip $MANDIR/hrry.1
