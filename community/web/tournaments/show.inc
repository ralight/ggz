<?php

$res = $database->exec("SELECT * FROM tournaments ORDER BY date DESC");

for($i = 0; $i < $database->numrows($res); $i++)
{
	$name = $database->result($res, $i, "name");
	$game = $database->result($res, $i, "game");
	$tid = $database->result($res, $i, "id");

	echo "<a href='index.php?page=show2&tid=$tid'>$name ($game)</a><br>\n";
}

?>
