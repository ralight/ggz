<?php

$conn = pg_connect("host=$dbhost dbname=$dbname user=$dbuser password=$dbpass");

$res = pg_exec($conn, "SELECT * FROM tournaments");

for($i = 0; $i < pg_numrows($res); $i++)
{
	$name = pg_result($res, $i, "name");
	$game = pg_result($res, $i, "game");
	$tid = pg_result($res, $i, "id");

	echo "<a href='index.php?page=show2&tid=$tid'>$name ($game)</a><br>\n";
}

?>
