<?php include("head.inc"); ?>

<table border=0 cellspacing=0 cellpadding=1><tr><td bgcolor='#000000'>
<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#00ff00'>

<b>Overview</b>
<br><br>

<?php

$conn = pg_connect("host=$dbhost dbname=$dbname user=$dbuser password=$dbpass");

$res = pg_exec($conn, "SELECT * FROM tournaments");

for($i = 0; $i < pg_numrows($res); $i++)
{
	$name = pg_result($res, $i, "name");
	$game = pg_result($res, $i, "game");
	$id = pg_result($res, $i, "id");

	echo "<a href='show2.php?id=$id'>$name ($game)</a><br>\n";
}

?>

<br><br>

<a href="index.php">Back to index</a>

<br><br>

</td></tr></table>
</td></tr></table>

<?php include("foot.inc"); ?>
