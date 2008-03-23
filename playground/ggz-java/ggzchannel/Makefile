ggzdir = /home/josef/sys

all: jancillary ggzchannel game

jancillary:
	gcc -O2 -Wall -Werror -fPIC -c jancillary.c -I $(ggzdir)/include
	gcc -shared -o libjancillary.so jancillary.o -L $(ggzdir)/lib -lggz

ggzchannel:
	javac GGZChannel.java

game:
	javac GameServer.java
	export LD_LIBRARY_PATH=. && java GameServer

clean:
	rm -f *.o *.so *.class

