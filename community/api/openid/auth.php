<?php

// we get here from index.php!
$identity = $_GET['openid_identity'];

ob_start();

if ($identity) :
	echo "* do openid authentication check: $identity\n";

	// FIXME: hm, all again?
	$idproviderurl = "http://api.ggzcommunity/openid/provider.php";

	$assoc_handle = $_GET['openid_assoc_handle'];
	$sig = $_GET['openid_sig'];
	$signed = $_GET['openid_signed'];

	// FIXME: all signed parameters as openid.*
	$signedlist = "";
	$list = explode(",", $signed);
	foreach ($list as $token)
	{
		if ($token == "mode") continue;
		if ($token == "assoc_handle") continue;
		$tokenvalue = $_GET["openid_$token"];
		echo "#TOKEN($token)=$tokenvalue...\n";
		if ($signedlist) :
			$signedlist .= "&";
		endif;
		$signedlist .= "openid.$token=$tokenvalue";
	}

	//$params = "openid.mode=checkid_immediate&openid.identity=$infourl&openid.return_to=$returl";
	$params = "openid.mode=check_authentication&openid.assoc_handle=$assoc_handle&openid.sig=$sig&openid.signed=$signed&$signedlist";
	#$idproviderurl = "$idproviderurl?$params";

	// now, call provider...
	echo "* call identity provider at $idproviderurl with params $params\n";
	//header("Location: $idproviderurl\n");
	//exit;

	 // FIXME (security): all exec() arguments must be quoted *properly* here and in test.php
	$responseparams = shell_exec("curl --limit-rate 6400 --max-time 10 -d '$params' $idproviderurl");
	if (!$responseparams) :
		echo "* curl call failed\n";
		exit;
	else :
		echo "* ok, parse auth result... [$responseparams]\n";
		$list = explode("\n", $responseparams);
		foreach ($list as $line)
		{
			echo "* parse $line\n";
			$linear = explode(":", $line);
			$token = $linear[0];
			$value = $linear[1];
			if ($token == "openid.mode") :
				// ignore...
			elseif ($token == "is_valid") :
				if ($value == "true") :
					echo "** IDENTIFIED **!\n";
					// now, proceed...
				else :
					echo "* forgery!\n";
					exit;
				endif;
			endif;
		}
	endif;
else :
	echo "* invalid action\n";
endif;

ob_end_flush();

?>
