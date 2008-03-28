CLASSPATH = ../jggzdmod/classes
B = classes

all: ttt

ttt:
	javac -cp $(CLASSPATH) -d $(B) CoffeeToe.java

clean:
	find . -name *.class | xargs rm

test:
	ggz-faketable runserver.sh /home/josef/sys/lib/ggz/ggz.ttt

