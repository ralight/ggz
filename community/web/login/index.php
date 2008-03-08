<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

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

if ($task == "reactivate") :
	//Locale::includefile("reactivate.inc");
	include("reactivate.inc");
	exit;
endif;

include_once("auth.php");

$input_user = $_POST["input_user"];
$input_pass = $_POST["input_pass"];
$input_realname = $_POST["input_realname"];
$input_email = $_POST["input_email"];
$input_encryption = $_POST["input_encryption"];
$input_activation = $_POST["input_activation"];
$input_token = $_POST["input_token"];

$param = "";

if (($input_user) && ($input_pass)) :
	if ($input_email) :
		$ret = Auth::register($input_user, $input_pass, $input_email, $input_realname);
		if ($ret) $param = "?register=done";
		else $param = "?register=failed";
	else :
		$ret = Auth::login($input_user, $input_pass);
		if (!$ret) $param = "?login=failed";
		else $param = "?login=done";
	endif;
elseif ($input_email) :
	$ret = Auth::resend($input_email, $input_encryption);
	if ($ret) $param = "?resend=done";
	else $param = "?resend=failed";
elseif ($input_activation) :
	$ret = Auth::activate($input_activation, $input_token);
	if ($ret) $param = "?activation=done";
	else $param = "?activation=failed";
else :
	Auth::logout();
endif;

header("Location: /$param");

?>
