#!/bin/sh

autoconf

if test x$NOCONFIGURE = x; then
	./configure $@
fi

