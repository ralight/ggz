<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<title>
GGZ Gaming Zone Ranking System
</title>
</head>
<body bgcolor='#00a000' link='#000000' alink='#000000' vlink='#000000'>

<?php

require_once(".htconf");

$id = @pg_connect("host=$dbhost dbname=$dbname user=$dbuser password=$dbpass");

if (($id) && ($ggzuser) && ($ggzpass)) :
	$res = pg_exec($id, "SELECT * FROM users WHERE password = '$ggzpass' AND handle = '$ggzuser'");
	if (($res) && (pg_numrows($res) == 1)) :
		$ggzuser = pg_result($res, 0, "handle");
	else :
		unset($ggzuser);
	endif;
endif;

include("top.php");

if ($id) :
	if ((!$register) && (!$lostpw)) :
		include("general.php");
	endif;
else :
	echo "Access to the database is currently not possible.";
endif;

?>

</body>
</html>

