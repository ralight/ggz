B = classes

all: jancillary jggzdmod ttt

jancillary:
	gcc -O2 -Wall -Werror -fPIC -c jancillary.c -I $(ggzdir)/include
	gcc -shared -o libjancillary.so jancillary.o -L $(ggzdir)/lib -lggz

jggzdmod:
	mkdir -p $(B)
	javac -d $(B) GGZChannel.java GGZDMod.java Handler.java Player.java Protocol.java Seat.java Spectator.java

ttt:
	javac -cp $(B) -d $(B) TicTacToe.java

clean:
	find . -name *.class | xargs rm

test:
	ggz-faketable runserver.sh /home/josef/sys/lib/ggz/ggz.ttt

