<?php

if ($task == "register") :
	Locale::includefile("register.inc");
	exit;
endif;

if ($task == "resend") :
	Locale::includefile("resend.inc");
	exit;
endif;

include("auth.php");

$input_user = $_POST["input_user"];
$input_pass = $_POST["input_pass"];
$input_realname = $_POST["input_realname"];
$input_email = $_POST["input_email"];

if (($input_user) && ($input_pass)) :
	if ($input_email) :
		Auth::register($input_user, $input_pass, $input_email);
	else :
		Auth::login($input_user, $input_pass);
	endif;
elseif ($input_email) :
	Auth::resend($input_email);
else :
	Auth::logout();
endif;

header("Location: /");

?>
