<?php

$id = pg_connect("host=mindx.dyndns.org dbname=ggz user=ggzd password=ggzd");

$input_user = $_POST["input_user"];
$input_pass = $_POST["input_pass"];

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
		setcookie("ggzuser", "$input_user");
	endif;
else :
	setcookie("ggzuser");
endif;

header("Location: index.php");

?>

