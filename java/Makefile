# In case 'ant' does not work, it can still be tried without the SVN task
# If this also fails, a simple 'make' invocation creates a basic application jar file

version = snapshot-0.99.5

DESTDIR =
prefix = /usr/local

all: compile

compile:
	ant -Dsvnant.unspec=true jar
	cp ggz-java-unspec.jar ggz-java-client.jar

makerun:
	cd src && javac */*/*.java */*/*/*.java
	cd src && jar -c -f ../bin/ggz-java-application.jar -m ../MANIFEST.MF */*/*.class */*/*/*.class */*/*.properties */*/*/*.properties

install:
	mkdir -p $(DESTDIR)$(prefix)/share/ggz/ggz-java
	cp ggz-java-client*.jar $(DESTDIR)$(prefix)/share/ggz/ggz-java/ggz-java-unspec.jar
	mkdir -p $(DESTDIR)$(prefix)/bin
	cp integration/ggz-java $(DESTDIR)$(prefix)/bin
	mkdir -p $(DESTDIR)$(prefix)/share/man/man6
	cp integration/ggz-java.6 $(DESTDIR)$(prefix)/share/man/man6

clean:
	rm -f src/*/*/*.class src/*/*/*/*.class
	rm -rf bin
	rm -f *.jar

dist:
	rm -rf ../ggz-java-$(version)
	cp -r ../java ../ggz-java-$(version)
	cd ../ggz-java-$(version) && svn-clean
	find ../ggz-java-$(version) -name .svn | xargs rm -rf
	cd .. && tar cvzf ggz-java-$(version).tar.gz ggz-java-$(version)
	rm -rf ../ggz-java-$(version)

