<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

include_once("auth.php");

if (!Auth::username()) :
	exit;
else:
	$ggzuser = Auth::username();
endif;

$user_photo = $_POST["user_photo"];
$user_realname = $_POST["user_realname"];
$user_email = $_POST["user_email"];
$user_gender = $_POST["user_gender"];
$user_country = $_POST["user_country"];
$user_password = $_POST["user_password"];
$user_pubkey = $_POST["user_pubkey"];
$user_blogfeed = $_POST["user_blogfeed"];
$user_longitude = $_POST["user_longitude"];
$user_latitude = $_POST["user_latitude"];
$user_language = $_POST["user_language"];

$res = $database->exec("SELECT * FROM userinfo WHERE handle = '%^'", array($ggzuser));
if (($res) && ($database->numrows($res) == 0)) :
	$res = $database->exec("INSERT INTO userinfo " .
		"(handle, photo, gender, country, longitude, latitude) VALUES " .
		"('%^', '', '', '', 0.0, 0.0)", array($ggzuser));
endif;

if ($password) :
	$res = $database->exec("UPDATE users SET ".
		"password = '%^' " .
		"WHERE handle = '%^'", array($user_password, $ggzuser));
	$md5pass = $user_password;
	setcookie("ggzuser", "$md5pass");
endif;
if ($settings) :
	$res = $database->exec("UPDATE userinfo SET ".
		"photo = '%^', gender = '%^', country = '%^', blogfeed = '%^' " .
		"WHERE handle = '%^'", array($user_photo, $user_gender, $user_country, $user_blogfeed, $ggzuser));
	$res = $database->exec("UPDATE userinfo SET ".
		"longitude = '%^', latitude = '%^' " .
		"WHERE handle = '%^'", array($user_longitude, $user_latitude, $ggzuser));
	$res = $database->exec("UPDATE users SET ".
		"name = '%^', email = '%^' " .
		"WHERE handle = '%^'", array($user_realname, $user_email, $ggzuser));
endif;
if ($pubkey) :
	$res = $database->exec("UPDATE userinfo SET ".
		"pubkey = '%^' " .
		"WHERE handle = '%^'", array($user_pubkey, $ggzuser));
endif;
if ($language) :
	Auth::setlanguage($user_language);
endif;

header("Location: index.php");

?>

