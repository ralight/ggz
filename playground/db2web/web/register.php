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

echo "<table border=0 cellspacing=0 cellpadding=1><tr><td bgcolor='#000000'>\n";
echo "<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#00ff00'>\n";

echo "<form action='register.php' method='POST'>";
echo "Username:<br>";
echo "<input name='form_username'>";
echo "</form>";

echo "</td></tr></table>\n";
echo "</td></tr></table>\n";

?>


</body>
</html>

