ggzdir = `ggz-config -c`/..

all:
	gcc \
		-o ggzresolver \
		ggzresolver.c \
		-I $(ggzdir)/include/ \
		-L $(ggzdir)/lib/ \
		-lggz

	gcc -O2 -Wall -o gai_a gai_a.c -lanl

testpure:
	./gai_a

testlibggz:
	./ggzresolver

