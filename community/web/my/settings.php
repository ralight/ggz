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

$res = pg_exec($id, "SELECT * FROM userinfo WHERE handle = '$ggzuser'");
if (($res) && (pg_numrows($res) == 0)) :
	$res = pg_exec($id, "INSERT INTO userinfo " .
		"(handle, photo, gender, country) VALUES " .
		"('$ggzuser', '', '', '')");
endif;

if ($password) :
	$res = pg_exec($id, "UPDATE users SET ".
		"password = '$user_password' " .
		"WHERE handle = '$ggzuser'");
	$md5pass = $user_password;
	setcookie("ggzuser", "$md5pass");
endif;
if ($settings) :
	$res = pg_exec($id, "UPDATE userinfo SET ".
		"photo = '$user_photo', gender = '$user_gender', country = '$user_country' " .
		"WHERE handle = '$ggzuser'");
	$res = pg_exec($id, "UPDATE users SET ".
		"name = '$user_realname', email = '$user_email' " .
		"WHERE handle = '$ggzuser'");
endif;
if ($pubkey) :
	$res = pg_exec($id, "UPDATE userinfo SET ".
		"pubkey = '$user_pubkey' " .
		"WHERE handle = '$ggzuser'");
endif;

header("Location: index.php");

?>

