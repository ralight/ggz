# In case 'ant' does not work, it can still be tried with an ant file without the SVN task
# If this also fails, a simple 'make' invocation creates a basic application jar file

all: makerun

nonsvnantrun:
	ant -f nosvn-build.xml

makerun:
	cd src && javac */*/*.java */*/*/*.java
	cd src && jar -c -f ../bin/ggz-java-application.jar -m ../MANIFEST.MF */*/*.class */*/*/*.class */*/*.properties */*/*/*.properties

