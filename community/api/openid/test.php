<?php

$infourl = $_POST['openid_url'];

ob_start();

if ($infourl) :
	echo "* do openid: $infourl\n";
	// now fetch and parse for openid.server, in /html/head/link[@rel='openid.server']
	// go to checkid_immediate, but append openid.return_to beforehand

	$tmpname = "";

	if (extension_loaded("curl")) :
		$tmphandle = tmpfile();

		$ch = curl_init();
		curl_setopt($ch, CURLOPT_URL, $infourl);
		#curl_setopt($ch, CURLOPT_HEADER, false);
		curl_setopt($ch, CURLOPT_FILE, $tmphandle);
		$ret = curl_exec($ch);
		curl_close($ch);

		if (!$ret) :
			echo "* curl failed on downloading info url\n";
			exit;
		endif;

		// do something ...
		// FIXME: curl never tested (missing extension)
		fclose($tmphandle);
	else :
		echo "* curl extension missing, fallback to wget\n";
		$tmpname = tempnam("/tmp", "openid_infourl");

		// FIXME: for security reasons, we need to limit time and size
		// NOTE: wget cannot do that, but curl can
		// e.g. curl --max-filesize 64000 --max-time 10 --output $tmpname $infourl
		// NOTE: since max-filesize can be circumvented, we need --limit-rate 6400
		// NOTE: --range could also be circumvented, so we don't use it
		//$ret = exec("wget $infourl -O $tmpname");
		$ret = exec("curl --limit-rate 6400 --max-time 10 --output $tmpname $infourl");

		// do something ...
		//unlink($tmpname);

		if ($ret) :
			echo "* download with wget/curl failed\n";
			exit;
		else :
			echo "* download with wget/curl succeeded [$tmpfile]\n";
		endif;
	endif;

	$idproviderurl = "";

	if ($tmpname) :
		$expr = "//*[local-name()='link'][@rel='openid.server']";
		$idproviderurl = shell_exec("xmllint --xmlout $tmpname | xmlstarlet sel -t -m \"$expr\" -v @href");
		if (!$idproviderurl) :
			// same as above, but using HTML parser for non-XHTML documents
			$idproviderurl = shell_exec("xmllint --html --xmlout $tmpname | xmlstarlet sel -t -m \"$expr\" -v @href");
		endif;
		$idproviderurl = trim($idproviderurl);
		unlink($tmpname);
	else :
		echo "* no downloaded file found\n";
		exit;
	endif;

	if (!$idproviderurl) :
		echo "* no identity provider url found\n";
		exit;
	endif;

	// FIXME: parse that from index.html if given as infourl, and check for errors
	//$idproviderurl = "http://api.ggzcommunity/openid/provider.php";

	$returl = "http://api.ggzcommunity/openid/";

	//$params = "openid.mode=checkid_immediate&openid.identity=$infourl&openid.return_to=$returl";
	$params = "openid.mode=checkid_setup&openid.identity=$infourl&openid.return_to=$returl";
	$idproviderurl = "$idproviderurl?$params";

	// now, call provider...
	echo "* call identity provider at $idproviderurl\n";
	header("Location: $idproviderurl\n");
	exit;
else :
	echo "* invalid action\n";
endif;

ob_end_flush();

?>
