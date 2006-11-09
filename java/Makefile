# In case 'ant' does not work, it can still be tried without the SVN task
# If this also fails, a simple 'make' invocation creates a basic application jar file

DESTDIR =
prefix = /usr/local

all: compile

compile:
	ant -Dsvnant.unspec=true application-jar

makerun:
	cd src && javac */*/*.java */*/*/*.java
	cd src && jar -c -f ../bin/ggz-java-application.jar -m ../MANIFEST.MF */*/*.class */*/*/*.class */*/*.properties */*/*/*.properties

install:
	mkdir -p $(DESTDIR)$(prefix)/share/ggz/ggz-java
	cp ggz-java-client*.jar $(DESTDIR)$(prefix)/share/ggz/ggz-java/ggz-java-client.jar
	mkdir -p $(DESTDIR)$(prefix)/bin
	cp integration/ggz-java $(DESTDIR)$(prefix)/bin
	mkdir -p $(DESTDIR)$(prefix)/share/man/man1
	cp integration/ggz-java.6 $(DESTDIR)$(prefix)/share/man/man1

clean:
	rm -f src/*/*/*.class src/*/*/*/*.class
	rm -rf bin
	rm -f *.jar

