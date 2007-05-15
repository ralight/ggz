<?php

$infourl = $_POST['openid_url'];

if ($infourl) :
	echo "* do openid: $infourl\n";
	// now fetch and parse for openid.server, in /html/head/link[@rel='openid.server']
	// go to checkid_immediate, but append openid.return_to beforehand

	// FIXME: parse that from index.html if given as infourl, and check for errors
	$idproviderurl = "http://api.ggzcommunity/openid/";

	$returl = "http://api.ggzcommunity/openid/";

	$params = "openid.mode=checkid_immediate&openid.identity=$infourl&openid.return_to=$returl";
	$idproviderurl = "$idproviderurl?$params";

	// now, call provider...
else :
	echo "* invalid action\n";
endif;

?>
