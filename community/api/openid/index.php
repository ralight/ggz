<?php

//print_r($_GET);
//echo "============\n";

if ($_GET['openid_mode'] == 'id_res') :
	// openid callback
	$setupurl = $_GET['openid_user_setup_url'];
// FIXME: those are only for checkid_immediate
//	if ($setupurl) :
//		// failed assertion, redirect user to setup URL
//		// FIXME: needs warning to user?
//		header("Redirect: $setupurl");
//		exit;
//	else :
		$identity = $_GET['openid_identity'];
		echo "## IDENTITY: $identity\n";
		// and now? verify using check_auth?

		include("auth.php");
		exit;
//	endif;
elseif ($_GET['openid_mode'] == 'error') :
	// openid error callback
	$error = $_GET['openid_error'];
	echo "ERROR: $error\n";
elseif ($_GET['openid_mode'] == 'cancel') :
	echo "CANCELLED!\n";
else :
	// normal call, no openid callback
	include("main.html");
endif;

?>
