CP=/usr/share/java/libgcj.jar
B=classes

all: tictactoe

tictactoe: ggzdmod
	jikes -bootclasspath $(B) -classpath $(CP) -d $(B) TicTacToe.java

ggzdmod:
	jikes -classpath $(CP) -d $(B) GGZDModSeat.java
	jikes -bootclasspath $(B) -classpath $(CP) -d $(B) GGZDMod.java

start:
	java -addclasspath classes Server

clean:
	find . -name *.class | xargs rm

