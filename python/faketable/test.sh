#!/bin/sh

ggzdir=`ggz-config -c`/..

# Work fine, more or less:
#./ggz-faketable $ggzdir/lib/ggzd/ggzd.tictactoe $ggzdir/lib/ggz/ggz.ttt
#./ggz-faketable $ggzdir/lib/ggzd/ggzd.dots $ggzdir/lib/ggz/ggz.dots
#./ggz-faketable $ggzdir/lib/ggzd/ggzd.connectx $ggzdir/bin/ggzboard

# Don't work for some reason:
#./ggz-faketable $ggzdir/lib/ggzd/ggzd.ggzcards $ggzdir/lib/ggz/ggz.ggzcards
# Reason: unknown
#./ggz-faketable $ggzdir/bin/civserver $ggzdir/bin/civclient
# Reason: (<errorsys>) Unable to create file (null)/.ggz/ggz-gtk.rc

# Test this one:
./ggz-faketable /usr/games/iagnod /usr/games/iagno

