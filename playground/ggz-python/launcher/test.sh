#!/bin/sh

ggzdir=`ggz-config -c`/..

./launchserver.py $ggzdir/lib/ggzd/ggzd.tictactoe $ggzdir/lib/ggz/ggz.ttt

