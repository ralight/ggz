===================================================
The GGZ Gaming Zone - Java Client - Snapshot 0.99.2
===================================================

The Java client is a port of the following C libraries.

libggz
ggz-client-libs 
ggz-gtk-client (redeveloped from scratch)
ggz-gtk-games/ggzcards (redeveloped from scratch)

To compile the project using the supplied build.xml file you'll need the 
following.

1. A version of the Java 1.4 SDK from Sun (or equivalent)
2. Apache Ant <http://ant.apache.org>
3. SvnAnt <http://subclipse.tigris.org/svnant.html>

I'm not sure which version of Ant you need but I was using 1.6.5 at the time 
of this writing. Alternatively, you can compile the Java source files manually 
using your favorite IDE or compiler since the Ant file doesn't do any special
setup. Once you have downloaded SvnAnt, you need to place the three jar files
in ANT_HOME/lib.

The build file creates two JAR files in this directory. One can be run as a 
Java application and the other is for deploying as an applet. The applet is 
currently available at <http://www.ggzcommunity.org/games/applet.php> so if 
you can't be bothered compiling the source or it's not working for you, you 
can check it out there. The applet is self signed so that it has permission 
to connect to live.ggzgamingzone.org but if you are deploying the applet on 
a site where it connects back to the server it was served from then it 
doesn't need to be signed.

Both the application and applet take the following parameters (remove the 
hyphen for the applet).

-uri    e.g. ggz://user:password@live.ggzgamingzone.org/Entry%20Room
-xmlin  stdout, stderr or a file to log server messages to.
-xmlout stdout, stderr or a file to log message we send to the server to.

xmlin and xmlout only log core client messages and not GGZ Cards messages. 
You can also enable debug logging by specifying the following Java System 
property on the command line. It uses standard JDK1.4 logging so it should 
be easy to find documentation on how to configure the log.properties file 
for your needs.

e.g.

java -Djava.util.logging.config.file=log.properties -jar ggz-java-client.jar


THEMES
======

Both the applet and application support themes and other customization via 
parameters. The applet gets the parameters from the <APPLET> tag whereas
the application reads them from the ggz-java.properties file. This file
is read from the current directory and you can also store uri, xmlin and 
xmlout parameters in this file rather than specifying them on the command 
line.

For a description of what each of the parameters do, see the comments in
ggz-java.properties.


If you encounter any bugs or have difficulty compiling, please send an email 
to the developer's mailing list.

  Project Page:	http://dev.ggzgamingzone.org/
  Dev Mailing List: ggz-dev@mail.ggzgamingzone.org

If you wish to help out the project with your coding and/or ideas, join`the 
developer's mailing list and start talking!  If you just want to play games, 
then enjoy the programs and we hope they work well for you!
