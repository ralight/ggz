<html>
<head>
<title>
GGZ Gaming Zone Ranking System
</title>
</head>
<body bgcolor='#00a000' link='#000000' alink='#000000' vlink='#000000'>

<?php

include("top.php");
include("stats.php");

$id = pg_connect("host=mindx.dyndns.org dbname=ggz user=ggzd password=ggzd");

echo "<table border=0 cellspacing=0 cellpadding=1><tr><td bgcolor='#000000'>\n";
echo "<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#00ff00'>\n";

echo "<b>Players:</b><br>\n";
$res = pg_exec($id, "SELECT handle FROM users ORDER BY handle ASC");
for ($i = 0; $i < pg_numrows($res); $i++)
{
	$handle = pg_result($res, $i, "handle");
	$handlefont = $handle;
	$pic = "player.png";
	if ($handle == $ggzuser) :
		$handlefont = "<b>$handle</b>";
		$pic = "you.png";
	endif;
	echo "<img src='icons/players/$pic' width=16 height=16>\n";
	echo "<a href='$SCRIPT_NAME?lookup=$handle&type=player'>$handlefont</a><br>\n";
}
echo "<br><br>\n";

echo "</td></tr></table>\n";

echo "</td></tr>\n";
echo "<tr><td bgcolor='#000000'>";

echo "<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#00ff00'>\n";

echo "<b>Games:</b><br>\n";
$res = pg_exec($id, "SELECT DISTINCT game FROM stats ORDER BY game ASC");
for ($i = 0; $i < pg_numrows($res); $i++)
{
	$game = pg_result($res, $i, "game");
	echo "<img src='icons/games/$game.png' width=16 height=16>\n";
	echo "<a href='$SCRIPT_NAME?lookup=$game&type=game'>$game</a><br>\n";
}
echo "<br><br>\n";

echo "</td></tr></table>\n";
echo "</td></tr></table>\n";

?>

</td><td valign=top>

<?php

if ($lookup) :

echo "<table border=0 cellspacing=0 cellpadding=1 width='100%'><tr><td bgcolor='#000000'>\n";
if ($type == "player") :
	echo "<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#ff9050'>\n";
elseif ($type == "game") :
	echo "<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#ffff00'>\n";
else :
	echo "<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#00ff00'>\n";
endif;

if ($type == "player") :
	if ($lookup == $ggzuser) :
		echo "<img src='icons/players/you.png' width=32 height=32>\n";
	else :
		echo "<img src='icons/players/player.png' width=32 height=32>\n";
	endif;
elseif ($type == "game") :
	echo "<img src='icons/games/$lookup.png' width=32 height=32>\n";
endif;
echo "<b>Statistics for '$lookup'</b><br><br>\n";

if ($type == "player") :
	stats_players($id, $lookup);
elseif ($type == "game") :
	stats_games($id, $lookup);
endif;

echo "<br><br>\n";

echo "</td></tr></table>\n";
echo "</td></tr></table>\n";

else :

echo "<table border=0 cellspacing=0 cellpadding=1 width='100%'><tr><td bgcolor='#000000'>\n";
echo "<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#00c0c0'>\n";

echo "<b>General server statistics</b><br>\n";

stats_statistics($id);

echo "<br><br>\n";

echo "</td></tr></table>\n";
echo "</td></tr></table>\n";

endif;

?>

</td></tr></table>

</body>
</html>

