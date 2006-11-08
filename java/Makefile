# In case 'ant' does not work, it can still be tried with an ant file without the SVN task
# If this also fails, a simple 'make' invocation creates a basic application jar file

DESTDIR = `pwd`/debian/ggz-java

#all: makerun
all: nonsvnantrun copyfiles

svnantrun:
	ant

nonsvnantrun:
	echo $(DESTDIR)
	ant -f nosvn-build.xml application-jar

makerun:
	cd src && javac */*/*.java */*/*/*.java
	cd src && jar -c -f ../bin/ggz-java-application.jar -m ../MANIFEST.MF */*/*.class */*/*/*.class */*/*.properties */*/*/*.properties

copyfiles:
	mkdir -p $(DESTDIR)/usr/share/ggz/ggz-java
	cp ggz-java-client*.jar $(DESTDIR)/usr/share/ggz/ggz-java/ggz-java-client.jar
	mkdir -p $(DESTDIR)/usr/bin
	cp $(DESTDIR)/../integration/ggz-java $(DESTDIR)/usr/bin
	mkdir -p $(DESTDIR)/usr/share/man/man1
	cp $(DESTDIR)/../integration/ggz-java.1 $(DESTDIR)/usr/share/man/man1

clean:
	rm -f src/*/*/*.class src/*/*/*/*.class
	rm -rf bin
	rm -f *.jar

