<?php

if ($_GET['openid.mode'] == 'id_res') :
	// openid callback
	$setupurl = $_GET['openid.user_setup_url'];
	if ($setupurl) :
		// failed assertion, redirect user to setup URL
		header("Redirect: $setupurl");
		exit;
	else :
		$identity = $_GET['openid.identity'];
		// and now?
	endif;
else :
	// normal call, no openid callback
	include("main.html");
endif;

?>
