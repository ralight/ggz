<?php

include_once("locale.php");
Locale::detect();

if ($task == "register") :
	Locale::includefile("register.inc");
	exit;
endif;

if ($task == "resend") :
	Locale::includefile("resend.inc");
	exit;
endif;

include_once("auth.php");

$input_user = $_POST["input_user"];
$input_pass = $_POST["input_pass"];
$input_realname = $_POST["input_realname"];
$input_email = $_POST["input_email"];
$input_encryption = $_POST["input_encryption"];

$param = "";

if (($input_user) && ($input_pass)) :
	if ($input_email) :
		$ret = Auth::register($input_user, $input_pass, $input_email);
		if ($ret) $param = "?register=done";
		else $param = "?register=failed";
	else :
		$ret = Auth::login($input_user, $input_pass);
		if (!$ret) $param = "?login=failed";
		else $param = "?login=done";
	endif;
elseif ($input_email) :
	Auth::resend($input_email, $input_encryption);
	$param = "?resend=done";
else :
	Auth::logout();
endif;

header("Location: /$param");

?>
