#!/bin/sh

export RUBYLIB=$PWD/../../ggz-ruby/ruggzmod:$PWD/../../ggz-ruby/ruggzdmod

faketable=`which ggz-faketable 2>/dev/null`

if [ -z $faketable ]; then
	echo "You need ggz-faketable installed!"
	echo "If you don't have it, please look into the source to see"
	echo "how to run the test manually..."
	exit 1
else
	ggz-faketable udpgameserver udpgameclient
fi

