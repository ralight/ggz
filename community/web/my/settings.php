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
$user_reason = $_POST["user_reason"];

$res = $database->exec("SELECT * FROM userinfo WHERE handle = '%^'", array($ggzuser));
if (($res) && ($database->numrows($res) == 0)) :
	$res = $database->exec("INSERT INTO userinfo " .
		"(handle, photo, gender, country, longitude, latitude) VALUES " .
		"('%^', '', '', '', 0.0, 0.0)", array($ggzuser));
endif;

if ($_GET["password"]) :
	$res = $database->exec("UPDATE users SET ".
		"password = '%^' " .
		"WHERE handle = '%^'", array($user_password, $ggzuser));
	$md5pass = $user_password;
	setcookie("ggzuser", "$md5pass");
endif;
if ($_GET["settings"]) :
	$res = $database->exec("UPDATE userinfo SET ".
		"photo = '%^', gender = '%^', country = '%^', blogfeed = '%^' " .
		"WHERE handle = '%^'", array($user_photo, $user_gender, $user_country, $user_blogfeed, $ggzuser));
	if (($user_longitude) && ($user_latitude)) :
		$res = $database->exec("UPDATE userinfo SET ".
			"longitude = '%^', latitude = '%^' " .
			"WHERE handle = '%^'", array($user_longitude, $user_latitude, $ggzuser));
	endif;
	$res = $database->exec("UPDATE users SET ".
		"name = '%^', email = '%^' " .
		"WHERE handle = '%^'", array($user_realname, $user_email, $ggzuser));
endif;
if ($_GET["pubkey"]) :
	$res = $database->exec("UPDATE userinfo SET ".
		"pubkey = '%^' " .
		"WHERE handle = '%^'", array($user_pubkey, $ggzuser));
endif;
if ($_GET["language"]) :
	Auth::setlanguage($user_language);
endif;
if ($_GET["applyforcake"]) :
	$res = $database->exec("UPDATE userinfo SET ".
		"request = '%^' " .
		"WHERE handle = '%^'", array("cake", $ggzuser));
endif;

header("Location: index.php");

?>

