<?php

require_once(".htconf");

$id = pg_connect("host=$dbhost dbname=$dbname user=$dbuser password=$dbpass");

$input_user = $_POST["input_user"];
$input_pass = $_POST["input_pass"];
$input_realname = $_POST["input_realname"];
$input_email = $_POST["input_email"];

function hex_decode($s)
{
	$ar = "0123456789abcdef";
	for ($i = 0; $i < strlen($s) / 2; $i++)
	{
		$value = strpos($ar, $s[$i * 2]) * 16 + strpos($ar, $s[$i * 2 + 1]);
		$ret .= chr($value);
	}
	return $ret;
}

if (($input_user) && ($input_email)) :
	$res = pg_exec($id, "SELECT * FROM users WHERE handle = '$input_user' AND email = '$input_email'");
	if (($res) && (pg_numrows($res) == 1)) :
		$pass = pg_result($res, 0, "password");
		mail($input_email, "GGZ Rankings: Your password",
			"The password for the user $input_user is $pass.");
	endif;
endif;

if (($input_user) && ($input_pass)) :
	$md5pass = $input_pass;
	$md5user = $input_user;

	$res = pg_exec($id, "SELECT * FROM users WHERE handle = '$input_user' AND password = '$md5pass'");
	if (($res) && (pg_numrows($res) == 1)) :
		if (!$register) :
			// login success
			setcookie("ggzuser", "$md5user");
			setcookie("ggzpass", "$md5pass");
		else :
			// registration failed
		endif;
	else :
		if (!$register) :
			// login failed
		else :
			// registration success
			$res = pg_exec($id, "INSERT INTO users (handle, password, permissions, name, email) " .
				"VALUES ('$input_user', '$md5pass', 7, '$input_realname', '$input_email')");
		endif;
	endif;
else :
	setcookie("ggzuser");
	setcookie("ggzpass");
endif;

header("Location: index.php");

?>

