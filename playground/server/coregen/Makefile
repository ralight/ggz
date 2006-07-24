all:
	./data.rb
	indent -bli0 -i8 -nlp -fc1 data.c
	gcc -Wall -O2 -Werror -c proto.c
	gcc -Wall -O2 -Werror -c data.c
	gcc -o test proto.o data.o -lpthread

