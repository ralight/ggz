<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<title>
GGZ Gaming Zone Ranking System
</title>
</head>
<body bgcolor='#00a000' link='#000000' alink='#000000' vlink='#000000'>

<?php

include("top.php");
require_once(".htconf");

$id = pg_connect("host=$dbhost dbname=$dbname user=$dbuser password=$dbpass");

if (!$register) :
	include("general.php");
endif;

?>

</body>
</html>

