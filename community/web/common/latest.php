<?php

function latest_matches()
{
	global $id;

	$res = pg_exec($id, "SELECT * FROM matches ORDER BY id DESC LIMIT 3");
	for ($i = 0; $i < pg_numrows($res); $i++)
	{
		$mid = pg_result($res, $i, "id");
		$game = pg_result($res, $i, "game");
		echo "<div class='menu'>\n";
		echo "<a href='/db/matches/?lookup=$mid' class='menuitem' title='$game'>$game #$mid</a>\n";
		echo "</div>\n";
	}
}

function latest_tournaments()
{
	global $id;

	$res = pg_exec($id, "SELECT * FROM tournaments ORDER BY id DESC LIMIT 2");
	for ($i = 0; $i < pg_numrows($res); $i++)
	{
		$tid = pg_result($res, $i, "id");
		$game = pg_result($res, $i, "game");
		echo "<div class='menu'>\n";
		echo "<a href='/db/tournaments/?lookup=$tid' class='menuitem' title='$game'>$game #$tid (Tournament)</a>\n";
		echo "</div>\n";
	}
}

function latest_forumposts()
{
	include("forums/.htconf");

	$conn = mysql_connect($conf_host, $conf_user, $conf_pass);
	mysql_select_db($conf_name, $conn);

	$res = mysql_query("SELECT * FROM phpbb_posts_text ORDER BY post_id DESC LIMIT 3", $conn);
	for ($i = 0; $i < mysql_num_rows($res); $i++)
	{
		$array = mysql_fetch_array($res);
		$subject = $array['post_subject'];
		echo "<div class='menu'>\n";
		echo "<a href='/forums/' class='menuitem' title='$subject'>$subject</a>\n";
		echo "</div>\n";
	}
}

function latest_blogs()
{
	$counter = 0;

	$f = fopen("blogs/planet.inc", "r");
	while($a = fgets($f, 1024))
	{
		$a = substr($a, 0, strlen($a) - 1);

		if (($old3 == "") && ($old2 == ":")) :
			$subject = substr($a, 0, strlen($a) - 4);
			$tag = htmlspecialchars($subject, ENT_QUOTES);
			echo "<div class='menu'>\n";
			echo "<a href='/blogs/' class='menuitem' title='$tag'>$subject</a>\n";
			echo "</div>\n";

			$counter += 1;
			if ($counter == 3) :
				break;
			endif;
		endif;

		$old3 = $old2;
		$old2 = $old1;
		$old1 = $a;
	}

	fclose($f);
}

?>
