all:
	jikes -classpath /usr/share/java/libgcj.jar TicTacToeClient.java

exec:
	gcj -c TicTacToeClient.java
	gcj -o tttjava --main=TicTacToeClient TicTacToeClient.o

clean:
	rm -f *.class
	rm -f *.o
	rm -f tttjava

