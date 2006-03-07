@rem This batch file signs the jar passed in as argument file using the
@rem kalido keystore previously created by the 'make_keystore.bat' batch file
@rem @rem Note: this requires the 'jarsigner' utility supplied with 
@rem Java 2 Standard Edition (J2SE) to be on the path. 
@rem Pratik Pandya, Kalido Ltd, March 2004 

set PATH="C:\Program Files\Java\jdk1.5.0_05\bin"
jar cfm ggz-java.jar ..\manifest\manifest.txt -C ..\bin .
jarsigner -keystore ggz_selfcert.keystore ggz-java.jar ggz_self_cert 
