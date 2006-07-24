all:
	gcc \
		-o ggzresolver \
		ggzresolver.c \
		-I /home/ggznew/BUILD/include/ \
		-L /home/ggznew/BUILD/lib/ \
		-lggz

	gcc -O2 -Wall -o gai_a gai_a.c -lanl

testpure:
	./gai_a

testlibggz:
	./ggzresolver

