	<div class="text">
	The results are listed below.
	<a href="playerfinder.php">Back</a> to search form.
	</div>
	<div class="text">

<?php

	$userlist = array();
	$userinfolist = array();

	$res = $database->exec("SELECT handle FROM users");

	for ($i = 0; $i < $database->numrows($res); $i++)
	{
		$handle = $database->result($res, $i, "handle");
		$userlist[$handle] = 1;
	}

	$res = $database->exec("SELECT handle, country, gender FROM userinfo");

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

	$res = $database->exec("SELECT * FROM permissions");

	for ($i = 0; $i < $database->numrows($res); $i++)
	{
		$handle = $database->result($res, $i, "handle");
		$admin = $database->result($res, $i, "rooms_admin");
		$bot = $database->result($res, $i, "chat_bot");

		if ($user_status == "admin") :
			if (!$admin) $userlist[$handle] = 0;
		endif;
		if ($user_status == "bot") :
			if (!$bot) $userlist[$handle] = 0;
		endif;
		if ($user_status == "registered") :
			if (($bot) || ($admin)) $userlist[$handle] = 0;
		endif;
	}

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

