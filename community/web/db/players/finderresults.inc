	<div class="text">
	The results are listed below.
	<a href="playerfinder.php">Back</a> to search form.
	</div>
	<div class="text">

<?php

	$userlist = array();
	$userinfolist = array();
	$userkarmalist = array();

	$res = $database->exec("SELECT handle FROM users", NULL);

	for ($i = 0; $i < $database->numrows($res); $i++)
	{
		$handle = $database->result($res, $i, "handle");
		$userlist[$handle] = 1;
	}

	$res = $database->exec("SELECT handle, country, gender FROM userinfo", NULL);

	for ($i = 0; $i < $database->numrows($res); $i++)
	{
		$handle = $database->result($res, $i, "handle");
		$country = $database->result($res, $i, "country");
		$gender = $database->result($res, $i, "gender");

		$userinfolist[$handle] = 1;
		if ($user_country) :
			if ($user_country != $country) $userlist[$handle] = 0;
		endif;
		if ($user_gender) :
			if ($user_gender != $gender) $userlist[$handle] = 0;
		endif;
	}

	if (($user_country) || ($user_gender)) :
		foreach ($userlist as $handle => $value)
		{
			if(!$userinfolist[$handle]) $userlist[$handle] = 0;
		}
	endif;

	$res = $database->exec("SELECT * FROM permissions", NULL);

	for ($i = 0; $i < $database->numrows($res); $i++)
	{
		$handle = $database->result($res, $i, "handle");
		$admin = $database->result($res, $i, "rooms_admin");
		$bot = $database->result($res, $i, "chat_bot");
		$host = $database->result($res, $i, "edit_tables");

		if (($host) && ($admin)) :
			$host = 0;
		endif;

		if ($user_status == "admin") :
			if (!$admin) $userlist[$handle] = 0;
		endif;
		if ($user_status == "bot") :
			if (!$bot) $userlist[$handle] = 0;
		endif;
		if ($user_status == "host") :
			if (!$host) $userlist[$handle] = 0;
		endif;
		if ($user_status == "registered") :
			if (($bot) || ($admin) || ($host)) $userlist[$handle] = 0;
		endif;
	}

	$res = $database->exec("SELECT karma.tohandle, COUNT(karma.tohandle) AS overall, " .
		"badkarma.badcount AS bad FROM karma LEFT JOIN " .
		"(SELECT tohandle, COUNT(karma) AS badcount FROM karma WHERE karma = 0 GROUP BY tohandle) " .
		"AS badkarma ON karma.tohandle = badkarma.tohandle " .
		"GROUP BY karma.tohandle, badkarma.badcount", NULL);
	for ($i = 0; $i < $database->numrows($res); $i++)
	{
		$handle = $database->result($res, $i, "tohandle");
		$overall_ratings = $database->result($res, $i, "overall");
		$bad_ratings = $database->result($res, $i, "bad");
		$karma = 10 * (1 - ($bad_ratings / $overall_ratings));

		$userkarmalist[$handle] = 1;
		if ($user_karma == "good") :
			if ($karma < 8) $userlist[$handle] = 0;
		endif;
		if ($user_karma == "medium") :
			if (($karma < 4) || ($karma > 7)) $userlist[$handle] = 0;
		endif;
		if ($user_karma == "bad") :
			if ($karma > 3) $userlist[$handle] = 0;
		endif;
	}

	if ($user_karma) :
		foreach ($userlist as $handle => $value)
		{
			if(!$userkarmalist[$handle]) $userlist[$handle] = 0;
		}
	endif;

	if ($user_veteran) :
		$res = $database->exec("SELECT * FROM users", NULL);

		for ($i = 0; $i < $database->numrows($res); $i++)
		{
			$handle = $database->result($res, $i, "handle");
			$firstlogin = $database->result($res, $i, "firstlogin");

			$mindays = 0;
			if ($user_veteran == "amateur") :
				$mindays = 14;
			endif;
			if ($user_veteran == "knownplayer") :
				$mindays = 30;
			endif;
			if ($user_veteran == "longtimeplayer") :
				$mindays = 60;
			endif;
			if ($user_veteran == "veteran") :
				$mindays = 180;
			endif;
			if ($user_veteran == "veteranhonours") :
				$mindays = 365;
			endif;

			$date = time();
			if (($date - $firstlogin) / 86500 < $mindays) :
				$userlist[$handle] = 0;
			endif;
		}
	endif;

	include_once("player.php");
	foreach ($userlist as $handle => $value)
	{
		if ($value) :
			//$player = new Player($handle);
			//$player->display();
			echo "Found player: <a href='/db/players/?lookup=$handle'>$handle</a><br>\n";
		endif;
	}

?>

	</div>

