@echo off
REM # Description:  This script produces a self-signed java signing certificate

echo ###########################################################################
echo # Note: This is NOT a 'pukka' certificate as issued by a 
echo # trusted authority like Verisign and therefore it is 
echo # intended for DEMO USE ONLY (e.g. for development, demos etc)
echo ########################################################################
echo

REM password = ggzcards for both keystore and cert

PATH=%JAVA_HOME%\bin;%PATH%

keytool -genkey -keystore ggz_selfcert.keystore -alias ggz_self_cert

keytool -export -keystore ggz_selfcert.keystore -alias ggz_self_cert -file ggz_self.cer


