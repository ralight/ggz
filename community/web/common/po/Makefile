all:
	@echo "make messages install"

install:
	mkdir -p de/LC_MESSAGES
	msgfmt -o de/LC_MESSAGES/ggzcommunity.mo de.po

messages:
	./extract.pl ../*.* > ggzcommunity.pot
	msgmerge de.po ggzcommunity.pot > tmp
	mv tmp de.po

#./extract.pl `find .. -name "*inc*"` > ggzcommunity.pot
