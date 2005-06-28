all: ggzgamingzone.mgp

ggzgamingzone.mgp: ggzgamingzone.mgp.in *.inc
	./mgp-create.pl $< > $@
	#iconv -f utf-8 -t iso-8859-1 $@ > tmp
	#mv tmp $@

view: ggzgamingzone.mgp
	mgp -F1,0,0 ggzgamingzone.mgp

export:
	mkdir -p html
	mgp -D html -E png ggzgamingzone.mgp

