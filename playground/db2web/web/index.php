<html>
<head>
<title>
GGZ Gaming Zone Ranking System
</title>
</head>
<body bgcolor='#00a000' link='#000000' alink='#000000' vlink='#000000'>

<?php

include("top.php");

$id = pg_connect("host=localhost dbname=ggz user=ggzd password=ggzd");

if (!$register) :
	include("general.php");
endif;

?>

</body>
</html>

