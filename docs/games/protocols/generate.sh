#!/bin/sh
#
# Creates the XHTML human-viewable documentation of ggzcomm protocol definitions
# (ex.: xsltproc ggzcommgen.xsl tictactoe.protocol > tictactoe.xhtml)

for p in *.protocol; do
	n=`echo $p | sed -e 's/\.protocol$//'`
	xsltproc ggzcommgen.xsl $p > $n.xhtml
done

