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

echo "<b>Teams:</b><br>\n";

$res = pg_exec($id, "SELECT teamname FROM teams ORDER BY teamname");
for ($i = 0; $i < pg_numrows($res); $i++)
{
	$team = pg_result($res, $i, "teamname");
	echo "<a href='$SCRIPT_NAME?lookup=$team&amp;type=team'>$team</a><br>\n";
}
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
	$handle = "---";
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
elseif ($type == "team") :
	echo "<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#f0e0c0'>\n";
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
elseif ($type == "team") :
	echo "<img src='ggzicons/teams/$lookup.png' width=32 height=32 alt='$lookup'>\n";
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
elseif ($type == "team") :
	stats_team($id, $lookup);
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
endif;
$res = pg_exec($id, "SELECT * FROM userinfo WHERE handle = '$ggzuser'");
if (($res) && (pg_numrows($res) == 1)) :
	$photo = pg_result($res, 0, "photo");
	$gender = pg_result($res, 0, "gender");
	$country = pg_result($res, 0, "country");
endif;

if (!$gender) :
	$gender = "N/A";
endif;
if (!$country) :
	$country = "N/A";
endif;

if ($photo) :
	echo "Photo:<br>\n";
	echo "<img src='$photo' width='100'>\n";
	echo "<br clear='all'>\n";
else :
	echo "Photo: none found<br>\n";
endif;
echo "Real name: $realname<br>\n";
echo "Email address: $email<br>\n";
echo "Gender: $gender<br>\n";
echo "Country: $country<br>\n";

echo "<br><br>\n";

echo "</td></tr></table>\n";
echo "</td></tr></table>\n";

echo "<br><br>\n";

echo "<table border=0 cellspacing=0 cellpadding=1 width='100%'><tr><td bgcolor='#000000'>\n";
echo "<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#00c0c0'>\n";

echo "<b>Change information</b><br>\n";

if ($realname == "N/A") :
	$realname = "";
endif;
if ($email == "N/A") :
	$email = "";
endif;
if ($gender == "N/A") :
	$gender = "";
endif;
if ($country == "N/A") :
	$country = "";
endif;

echo "<table><tr><td bgcolor='#00a0a0'>\n";

echo "<form action='settings.php?settings=1' method='POST'>\n";
echo "<table>\n";
echo "<tr><td>Photo:</td><td><input type='text' name='user_photo' value='$photo'></td></tr>\n";
echo "<tr><td>Real name:</td><td><input type='text' name='user_realname' value='$realname'></td></tr>\n";
echo "<tr><td>Email address:</td><td><input type='text' name='user_email' value='$email'></td></tr>\n";
echo "<tr><td>Gender:</td><td><input type='text' name='user_gender' value='$gender'></td></tr>\n";
echo "<tr><td>Country:</td><td><input type='text' name='user_country' value='$country'></td></tr>\n";
echo "<tr><td></td><td><input type='submit' value='Change'></td></tr>\n";
echo "</table>\n";
echo "</form>\n";

echo "</td></tr></table>\n";

echo "<br><br>\n";

echo "<b>Password alteration</b><br>\n";

echo "<table><tr><td bgcolor='#00a0a0'>\n";

echo "<form action='settings.php?password=1' method='POST'>\n";
echo "<table>\n";
echo "<tr><td>Password:</td><td><input type='password' name='user_password' value=''></td></tr>\n";
echo "<tr><td></td><td><input type='submit' value='Change'></td></tr>\n";
echo "</table>\n";
echo "</form>\n";

echo "</td></tr></table>\n";

echo "<br><br>\n";

echo "</td></tr></table>\n";
echo "</td></tr></table>\n";

echo "<br><br>\n";

echo "<table border=0 cellspacing=0 cellpadding=1 width='100%'><tr><td bgcolor='#000000'>\n";
echo "<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#00c0c0'>\n";

echo "<b>Teams and Clans</b><br>\n";

echo "<table><tr><td bgcolor='#00a0a0'>\n";

echo "<form action='teams.php?create=1' method='POST'>\n";
echo "<table>\n";
echo "<tr><td>Team name:</td><td><input type='input' name='team_name' value=''></td></tr>\n";
echo "<tr><td></td><td><input type='submit' value='Create'></td></tr>\n";
echo "</table>\n";
echo "</form>\n";

echo "</td></tr></table>\n";

echo "<br><br>\n";

echo "<b>Team membership</b><br>\n";

echo "<table><tr><td bgcolor='#00a0a0'>\n";

echo "<form action='teams.php?join=1' method='POST'>\n";
echo "<table>\n";
echo "<tr><td>Team name:</td><td>\n";
echo "<select name='team_name'>\n";
$res = pg_exec($id, "SELECT * FROM teams");
for ($i = 0; $i < pg_numrows($res); $i++)
{
	$teamname = pg_result($res, $i, "teamname");
	echo "<option>$teamname</option>\n";
}
echo "</select>\n";
echo "</td></tr>\n";
echo "<tr><td></td><td><input type='submit' value='Join'>\n";
echo "<input type='submit' value='Leave'></td></tr>\n";
echo "</table>\n";
echo "</form>\n";

echo "</td></tr></table>\n";

echo "<br><br>\n";

echo "</td><td bgcolor='#00c0c0' valign='top'>\n";

echo "Current teams:<br><br>\n";

$res = pg_exec($id, "SELECT * FROM teammembers WHERE handle = '$ggzuser' AND role LIKE '%member%'");
for ($i = 0; $i < pg_numrows($res); $i++)
{
	$teamname = pg_result($res, $i, "teamname");
	$role = pg_result($res, $i, "role");

	$color = "silver";
	$number = 1;
	$attribute = "";
	$title = "";

	if (strstr($role, "founder")) :
		$title .= "Founder & ";
		$attribute = "s";
	endif;
	if (strstr($role, "leader")) :
		$title .= "Leader & ";
		$color = "gold";
		if ($attribute != "s") :
			$attribute = "d";
		endif;
	endif;
	if (strstr($role, "vice")) :
		$title .= "Vice Leader & ";
		$color = "gold";
	endif;

	$title = substr($title, 0, strlen($title) - 2);

	echo "$teamname\n";
	for ($i = 0; $i < $number; $i++)
	{
		echo "<img src='ggzicons/rankings/$color$attribute.png' title='$title'>\n";
	}
	echo "<br>\n";
}


echo "Bla\n";
echo "<img src='ggzicons/rankings/golds.png' title='Multiple Winner & Leader & Founder'>\n";
echo "<img src='ggzicons/rankings/golds.png' title='Multiple Winner & Leader & Founder'>\n";
echo "<img src='ggzicons/rankings/golds.png' title='Multiple Winner & Leader & Founder'>\n";
echo "<br>\n";
echo "Kuh\n";
echo "<img src='ggzicons/rankings/gold.png' title='Winner & Vice Leader'>\n";
echo "<img src='ggzicons/rankings/gold.png' title='Winner & Vice Leader'>\n";
echo "<br>\n";
echo "Narf\n";
echo "<img src='ggzicons/rankings/silver.png' title='Winner'>\n";
echo "<img src='ggzicons/rankings/silver.png' title='Winner'>\n";
echo "<br>\n";
echo "Ruffle\n";
echo "<img src='ggzicons/rankings/silver.png' title='Member'>\n";
echo "<br>\n";
echo "TheKing\n";
echo "<img src='ggzicons/rankings/rubin.png' title='Site Admin'>\n";
echo "<br>\n";

$res = pg_exec($id, "SELECT * FROM teammembers WHERE handle = '$ggzuser' AND role NOT LIKE '%member%'");

if (pg_numrows($res) > 0) :
	echo "<br>\n";
	echo "Pending:<br>\n";

	for ($i = 0; $i < pg_numrows($res); $i++)
	{
		$teamname = pg_result($res, $i, "teamname");

		echo "$teamname (pending...)<br>\n";
	}
endif;

echo "</td></tr></table>\n";
echo "</td></tr></table>\n";

echo "<br><br>\n";



echo "<table border=0 cellspacing=0 cellpadding=1 width='100%'><tr><td bgcolor='#000000'>\n";
echo "<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#00c0c0'>\n";

$teamname = 'fooclan';

echo "<b>Team applications for $teamname</b><br>\n";

echo "<table><tr><td bgcolor='#00a0a0'>\n";

echo "<form action='teams.php?approve=1' method='POST'>\n";
echo "<table>\n";
echo "<tr><td>Player:</td><td>\n";
echo "<input type='hidden' name='team_name' value='$teamname'>\n";
echo "<select name='player_name'>\n";
$res = pg_exec($id, "SELECT * FROM teammembers WHERE teamname = '$teamname' AND role = ''");
for ($i = 0; $i < pg_numrows($res); $i++)
{
	$playername = pg_result($res, $i, "handle");
	echo "<option>$playername</option>\n";
}
echo "</select>\n";
echo "</td></tr>\n";
echo "<tr><td>Approval:</td><td>\n";
echo "<select name='player_approval'>\n";
echo "<option>approved</option>\n";
echo "<option>declined</option>\n";
echo "</select>\n";
echo "</td></tr>\n";
echo "<tr><td></td><td><input type='submit' value='Approve'></td></tr>\n";
echo "</table>\n";
echo "</form>\n";

echo "</td></tr></table>\n";

echo "<br><br>\n";

echo "</td></tr></table>\n";
echo "</td></tr></table>\n";

echo "<br><br>\n";




echo "<table border=0 cellspacing=0 cellpadding=1 width='100%'><tr><td bgcolor='#000000'>\n";
echo "<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#00c0c0'>\n";

$teamname = 'fooclan';

echo "<b>Team management for $teamname</b><br>\n";

echo "<table><tr><td bgcolor='#00a0a0'>\n";

echo "<form action='teams.php?manage=1' method='POST'>\n";
echo "<table>\n";
echo "<tr><td>Player:</td><td>\n";
echo "<input type='hidden' name='team_name' value='$teamname'>\n";
echo "<select name='player_name'>\n";
$res = pg_exec($id, "SELECT * FROM teammembers WHERE teamname = '$teamname' AND role <> ''");
for ($i = 0; $i < pg_numrows($res); $i++)
{
	$playername = pg_result($res, $i, "handle");
	$role = pg_result($res, $i, "role");
	echo "<option value='$playername'>$playername ($role)</option>\n";
}
echo "</select>\n";
echo "</td></tr>\n";
echo "<tr><td>Assign role:</td><td>\n";
echo "<select name='player_role'>\n";
echo "<option value='member'>Member</option>\n";
echo "<option value='vice'>Vice Leader</option>\n";
echo "<option value='leader'>Leader</option>\n";
echo "</select>\n";
echo "</td></tr>\n";
echo "<tr><td></td><td><input type='submit' value='Change'></td></tr>\n";
echo "</table>\n";
echo "</form>\n";

echo "</td></tr></table>\n";

echo "<br><br>\n";

echo "</td></tr></table>\n";
echo "</td></tr></table>\n";

echo "<br><br>\n";





echo "<table border=0 cellspacing=0 cellpadding=1 width='100%'><tr><td bgcolor='#000000'>\n";
echo "<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#00c0c0'>\n";

echo "<b>Rankings</b><br>\n";

echo "Global:\n";
echo "<img src='ggzicons/rankings/cupgolda.png' title='Rank 1'>\n";
echo "<img src='ggzicons/rankings/cupsilvera.png' title='Rank 2'>\n";
echo "<img src='ggzicons/rankings/cupbronzea.png' title='Rank 3'>\n";
echo "<br>\n";
echo "TicTacToe:\n";
echo "<img src='ggzicons/rankings/cupgoldg.png' title='Rank 1'>\n";
echo "<img src='ggzicons/rankings/cupsilverg.png' title='Rank 2'>\n";
echo "<img src='ggzicons/rankings/cupbronzeg.png' title='Rank 3'>\n";
echo "<br>\n";
echo "Tournament class Bar:\n";
echo "<img src='ggzicons/rankings/cupsilvert.png' title='Rank 2'>\n";
echo "<br>\n";
echo "Tournament Foo of class Bar:\n";
echo "<img src='ggzicons/rankings/cupgold.png' title='1st place'>\n";
echo "<br>\n";
echo "Game of gametype TicTacToe:\n";
echo "<img src='ggzicons/rankings/coingold.png' title='winner'>\n";
echo "<br>\n";

echo "</td></tr></table>\n";
echo "</td></tr></table>\n";

endif;

endif;

?>

</td></tr></table>

