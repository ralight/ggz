<?php

include("stats.php");

echo "<table border=0 cellspacing=0 cellpadding=1><tr><td bgcolor='#000000'>\n";
echo "<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#00ff00'>\n";

echo "<b>Live games:</b><br>\n";
echo "<a href='index.php?type=live'>View games</a>\n";
echo "<br><br>\n";

echo "</td></tr></table>\n";

echo "</td></tr>\n";
echo "<tr><td bgcolor='#000000'>";

echo "<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#00ff00'>\n";

echo "<b>Players:</b><br>\n";
for ($i = 65; $i < 91; $i++)
{
	$c = chr($i);
	echo "<a href='index.php?handle=$c'>$c</a>";
	if (!($i % 9)) :
		echo "<br>\n";
	endif;
}
echo "<a href='index.php?handle=ALL'>All players</a><br>\n";
if ($ggzuser) :
	echo "<a href='index.php?handle=FRIENDS'>Friends</a><br>\n";
endif;
echo "<br>\n";

if (!$handle) :
	$handle = "A";
endif;
if ($handle != "ALL") :
	$handle = strtolower($handle);
	$limit = "WHERE lower(handle) LIKE '$handle%'";
else :
	$limit = "";
endif;
$res = pg_exec($id, "SELECT handle FROM users $limit ORDER BY handle");
for ($i = 0; $i < pg_numrows($res); $i++)
{
	$handle = pg_result($res, $i, "handle");
	$handlefont = $handle;
	$pic = "player.png";
	if ($handle == $ggzuser) :
		$handlefont = "<b>$handle</b>";
		$pic = "you.png";
	endif;
	echo "<img src='ggzicons/players/$pic' width=16 height=16 alt='player'>\n";
	echo "<a href='$SCRIPT_NAME?lookup=$handle&amp;type=player'>$handlefont</a><br>\n";
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
	echo "<img src='ggzicons/games/$game.png' width=16 height=16 alt='game'>\n";
	echo "<a href='$SCRIPT_NAME?lookup=$game&amp;type=game'>$game</a><br>\n";
}
echo "<br><br>\n";

echo "</td></tr></table>\n";
echo "</td></tr></table>\n";

?>

</td><td valign=top>

<?php

if (($lookup) || ($type)) :

echo "<table border=0 cellspacing=0 cellpadding=1 width='100%'><tr><td bgcolor='#000000'>\n";
if ($type == "player") :
	echo "<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#ff9050'>\n";
elseif ($type == "game") :
	echo "<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#ffff00'>\n";
elseif ($type == "live") :
	echo "<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#9f4ff0'>\n";
else :
	echo "<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#00ff00'>\n";
endif;

if ($type == "player") :
	if ($lookup == $ggzuser) :
		echo "<img src='ggzicons/players/you.png' width=32 height=32 alt='you'>\n";
	else :
		echo "<img src='ggzicons/players/player.png' width=32 height=32 alt='player'>\n";
	endif;
elseif ($type == "game") :
	echo "<img src='ggzicons/games/$lookup.png' width=32 height=32 alt='$lookup'>\n";
endif;
if ($lookup) :
	echo "<b>Statistics for '$lookup'</b><br><br>\n";
endif;
if ($type == "live") :
	echo "<b>Live game overview</b><br><br>\n";
endif;

if ($type == "player") :
	stats_players($id, $lookup);
elseif ($type == "game") :
	stats_games($id, $lookup);
elseif ($type == "live") :
	stats_live($ggzhost);
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

if ($ggzuser) :

echo "<br><br>\n";

echo "<table border=0 cellspacing=0 cellpadding=1 width='100%'><tr><td bgcolor='#000000'>\n";
echo "<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#00c0c0'>\n";

echo "<b>Personal information for $ggzuser</b><br>\n";

$res = pg_exec($id, "SELECT * FROM users WHERE handle = '$ggzuser'");
if (($res) && (pg_numrows($res) == 1)) :
	$realname = pg_result($res, 0, "name");
	$email = pg_result($res, 0, "email");
	$photo = pg_result($res, 0, "photo");
endif;

if ($photo) :
	echo "Photo:<br>\n";
	echo "<img src='$photo' width='100'>\n";
	echo "<br clear='all'>\n";
else :
	echo "Photo: none found\n";
endif;
echo "Real name: $realname<br>\n";
echo "Email address: $email<br>\n";

echo "<br><br>\n";

echo "</td></tr></table>\n";
echo "</td></tr></table>\n";

endif;

endif;

?>

</td></tr></table>

