<?php

$id = pg_connect("host=localhost dbname=ggz user=ggzd password=ggzd");

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

if (($input_user) && ($input_pass)) :
	$md5pass = md5($input_pass);
	$md5pass = hex_decode($md5pass);
	$md5pass = base64_encode($md5pass);

	$res = pg_exec($id, "SELECT * FROM users WHERE handle = '$input_user' AND password = '$md5pass'");
	if (($res) && (pg_numrows($res) == 1)) :
		if (!$register) :
			// login success
			setcookie("ggzuser", "$input_user");
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
endif;

header("Location: index.php");

?>

