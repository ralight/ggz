CP=/usr/share/java/libgcj.jar
B=classes

all:
	#guavac server.java
	jikes -classpath $(CP) -d $(B) server.java

start:
	java -addclasspath classes Server

clean:
	rm -f *.class

