#!/bin/sh

##### Instructions for systems without ggz-faketable installed
#
## Use 'TicTacToe' or 'RubyToe' for the game type
#export PATH=.:$PATH
#ggz-wrapper -g RubyToe -f text -s localhost -d ""
## Note: ggzd changes dirs after start so we need 'PWD' instead of '.'
#export RUBYLIB=$PWD
#ggzd -F

export RUBYLIB=$PWD/../ruggzmod:$PWD/../ruggzdmod

faketable=`which ggz-faketable 2>/dev/null`

if [ -z $faketable ]; then
	echo "You need ggz-faketable installed!"
	echo "If you don't have it, please look into the source to see"
	echo "how to run the test manually..."
	exit 1
else
	ggz-faketable rubytoe rubytoeclient
fi

