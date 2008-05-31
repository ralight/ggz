<?php

function latest_matches()
{
	global $database;

	$res = $database->exec("SELECT * FROM matches ORDER BY id DESC LIMIT 3", NULL);
	for ($i = 0; $i < $database->numrows($res); $i++)
	{
		$mid = $database->result($res, $i, "id");
		$game = $database->result($res, $i, "game");

		echo "<div class='menu'>\n";
		echo "<a href='/db/matches/?lookup=$mid' class='menuitem' title='$game'>$game #$mid</a>\n";
		echo "</div>\n";
	}
	if ($i == 0) :
		echo "No matches found.<br>\n";
	endif;
}

function latest_tournaments()
{
	global $database;

	$res = $database->exec("SELECT * FROM tournaments ORDER BY id DESC LIMIT 2", NULL);
	for ($i = 0; $i < $database->numrows($res); $i++)
	{
		$tid = $database->result($res, $i, "id");
		$game = $database->result($res, $i, "game");

		echo "<div class='menu'>\n";
		echo "<a href='/db/tournaments/?lookup=$tid' class='menuitem' title='$game'>$game #$tid (Tournament)</a>\n";
		echo "</div>\n";
	}
	if ($i == 0) :
		echo "No tournaments found.<br>\n";
	endif;
}

function latest_forumposts()
{
	$ret = @include("forums/.htconf");
	if(!$ret) $ret = @include("../forums/.htconf");
	if(!$ret) $ret = @include("../../forums/.htconf");

	$phpbb = new Database("postgresql");
	$phpbb->connect($conf_host, $conf_name, $conf_user, $conf_pass, $conf_port);

	if (!$phpbb->id) :
		echo "Forum not reachable.\n";
		return;
	endif;

	$res = $phpbb->exec("SELECT * FROM phpbb_posts_text AS a, phpbb_posts AS b " .
		"WHERE a.post_id = b.post_id ORDER BY a.post_id DESC LIMIT 3", NULL);
	for ($i = 0; $i < $phpbb->numrows($res); $i++)
	{
		$subject = $phpbb->result($res, $i, "post_subject");
		$id = $phpbb->result($res, $i, "topic_id");

		$tag = htmlspecialchars($subject, ENT_QUOTES);

		echo "<div class='menu'>\n";
		echo "<a href='/forums/phpBB2/viewtopic.php?t=$id' class='menuitem' title='$tag' target='_BLANK'>$subject</a>\n";
		echo "</div>\n";
	}
}

function latest_blogs()
{
	$counter = 0;

	$f = @fopen("blogs/planet.inc", "r");
	if(!$f) $f = @fopen("../blogs/planet.inc", "r");

	if (!$f) :
		echo "Blogs not reachable.\n";
		return;
	endif;

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
