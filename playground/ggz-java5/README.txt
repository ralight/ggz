This is the source code for the Java port of the following C++ GGZ libraries.

libggz
ggz-client-libs 
ggz-gtk-client (redeveloped from scratch)
ggz-gtk-games/ggzcards (redeveloped from scratch)

To compile the project you'll need a version of the Java 5 SDK from Sun as well as Apache Ant. I'm not sure which version you need but I was using 1.6.5 at the time of this writing.

The build file creates a JAR file in this directory that can be run either as a Java application or applet. Bot the application and applet take the following parameters (remove the double hyphen for the applet).

--uri    e.g. ggz://user:password@live.ggzgamingzone.org/Entry%20Room
--xmlIn  stdout,stderr or a file to log server messages to.
--xmlOut stdout,stderr or a file to log message we send to the server to.

xmlIn and xmlOut only log core client messages and not GGZ Cards messages. You can also enable debug logging by specifying the following Java System property on the command line. It uses standard JDK1.4 logging so it should be easy to find documentation on how to configure the log.properties file for your needs.

e.g.

java -Djava.util.logging.config.file=log.properties -jar ggz-java-client.jar


If you have any questions feel free to email me at xx404@msn.com.