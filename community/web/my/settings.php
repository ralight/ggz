<?php

include_once("database.php");
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

$res = $database->exec("SELECT * FROM userinfo WHERE handle = '$ggzuser'");
if (($res) && ($database->numrows($res) == 0)) :
	$res = $database->exec("INSERT INTO userinfo " .
		"(handle, photo, gender, country) VALUES " .
		"('$ggzuser', '', '', '')");
endif;

if ($password) :
	$res = $database->exec("UPDATE users SET ".
		"password = '$user_password' " .
		"WHERE handle = '$ggzuser'");
	$md5pass = $user_password;
	setcookie("ggzuser", "$md5pass");
endif;
if ($settings) :
	$res = $database->exec("UPDATE userinfo SET ".
		"photo = '$user_photo', gender = '$user_gender', country = '$user_country', blogfeed = '$user_blogfeed' " .
		"WHERE handle = '$ggzuser'");
	$res = $database->exec("UPDATE users SET ".
		"name = '$user_realname', email = '$user_email' " .
		"WHERE handle = '$ggzuser'");
endif;
if ($pubkey) :
	$res = $database->exec("UPDATE userinfo SET ".
		"pubkey = '$user_pubkey' " .
		"WHERE handle = '$ggzuser'");
endif;

header("Location: index.php");

?>

