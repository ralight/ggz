all:
	jikes -classpath /usr/share/java/libgcj.jar TicTacToeClient.java

exec:
	gcj -c TicTacToeClient.java
	gcj -o tttjava --main=TicTacToeClient TicTacToeClient.o

install:
	cp *.class java.sh /home/ggz/BUILD//lib/ggz
	ggz-config --force --install --modfile=module.dsc

clean:
	rm -f *.class
	rm -f *.o
	rm -f tttjava

