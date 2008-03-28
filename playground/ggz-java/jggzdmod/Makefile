B = classes

all: jancillary jggzdmod

jancillary:
	gcc -O2 -Wall -Werror -fPIC -c jancillary.c -I $(ggzdir)/include
	gcc -shared -o libjancillary.so jancillary.o -L $(ggzdir)/lib -lggz

jggzdmod:
	mkdir -p $(B)
	javac -d $(B) GGZChannel.java GGZChannelPoller.java \
		GGZDMod.java Handler.java Player.java Protocol.java Seat.java Spectator.java Score.java

clean:
	find . -name *.class | xargs rm

