<?php

include_once("auth.php");
include_once("countrylist.php");
include_once("genderlist.php");

class Player
{
	var $handle;
	var $realname;
	var $email;
	var $photo;
	var $gender;
	var $country;

	function Player($ggzuser)
	{
		global $database;

		$this->handle = $ggzuser;

		$res = $database->exec("SELECT * FROM users WHERE handle = '$ggzuser'");
		if (($res) && ($database->numrows($res) == 1)) :
			$this->realname = $database->result($res, 0, "name");
			$this->email = $database->result($res, 0, "email");
		endif;
		$res = $database->exec("SELECT * FROM userinfo WHERE handle = '$ggzuser'");
		if (($res) && ($database->numrows($res) == 1)) :
			$this->photo = $database->result($res, 0, "photo");
			$this->gender = $database->result($res, 0, "gender");
			$this->country = $database->result($res, 0, "country");
		endif;
	}

	function display()
	{
		if ($this->photo) :
			echo "Photo:<br>\n";
			echo "<img src='$this->photo' height='64'>\n";
			echo "<br clear='all'>\n";
		else :
			echo "Photo: none found<br>\n";
		endif;
		echo "Real name: $this->realname<br>\n";
		echo "Email address: $this->email<br>\n";
		if ($this->gender) :
			$g = new Gender();
			$gendername = $g->name($this->gender);
			$pic = $g->graphics($this->gender);
			echo "Gender: $pic $gendername";
		else :
			echo "Gender: (undisclosed)";
		endif;
		echo "<br>\n";
		if ($this->country) :
			$c = new Country();
			$countryname = $c->name($this->country);
			$flag = $c->flag($this->country);
			echo "Country: $flag $countryname";
		else :
			echo "Country: (undisclosed)";
		endif;
		echo "<br>\n";
	}

	function permissions()
	{
		global $database;

		$ggzuser = $this->handle;

		$res = $database->exec("SELECT * FROM permissionmasks WHERE handle = '$ggzuser'");
		if (($res) && ($database->numrows($res) > 0)) :
			$admin = $database->result($res, 0, "admin_mask");
			$anon = $database->result($res, 0, "anon_mask");
			$normal = $database->result($res, 0, "normal_mask");

			if ($admin == "t") :
				$permission = "Administrator";
			elseif ($normal == "t") :
				$permission = "Registered player";
			else :
				$permission = "Incomplete authority";
			endif;
		endif;

		echo "Permissions: $permission<br>\n";

		$res = $database->exec("SELECT * FROM permissions WHERE handle = '$ggzuser'");
		if (($res) && ($database->numrows($res) > 0)) :
			$join_table = $database->result($res, 0, "join_table");
			$launch_table = $database->result($res, 0, "launch_table");
			$rooms_login = $database->result($res, 0, "rooms_login");
			$rooms_admin = $database->result($res, 0, "rooms_admin");
			$chat_announce = $database->result($res, 0, "chat_announce");
			$chat_bot = $database->result($res, 0, "chat_bot");
			$no_stats = $database->result($res, 0, "no_stats");
			$edit_tables = $database->result($res, 0, "edit_tables");
		endif;

		echo "<br>\n";
		echo "Can join tables: $join_table<br>\n";
		echo "Can launch tables: $launch_table<br>\n";
		echo "Can log into rooms: $rooms_login<br>\n";
		echo "Room administrator: $rooms_admin<br>\n";
		echo "Can broadcast chat messages: $chat_announce<br>\n";
		echo "Is a chatbot: $chat_bot<br>\n";
		echo "Excluded from statistics: $no_stats<br>\n";
		echo "Can edit tables: $edit_tables<br>\n";
	}

	function items($fullview = false)
	{
		global $database;

		$ggzuser = $this->handle;

		if ($fullview) :
			echo "<div class='text'>\n";
		endif;

		$res = $database->exec("SELECT teams.teamname, teams.fullname " .
			"FROM teammembers, teams " .
			"WHERE teams.teamname = teammembers.teamname AND handle = '$ggzuser' AND role <> ''");
		if (($res) && ($database->numrows($res) > 0)) :
			echo "<h2>Teams</h2>\n";
			for ($i = 0; $i < $database->numrows($res); $i++)
			{
				$team = $database->result($res, $i, "teamname");
				$teamname = $database->result($res, $i, "fullname");

				if (!$teamname) :
					$teamname = "($team)";
				endif;
				echo "<a href='/db/teams/?lookup=$team'>$teamname</a>\n";
				echo "<br>\n";
			}
		endif;

		if ($fullview) :
			echo "</div>\n";
			echo "<div class='text'>\n";
		endif;

		$res = $database->exec("SELECT * FROM tournaments WHERE organizer = '$ggzuser'");
		if (($res) && ($database->numrows($res) > 0)) :
			echo "<h2>Tournaments</h2>\n";
			for ($i = 0; $i < $database->numrows($res); $i++)
			{
				$name = $database->result($res, $i, "name");
				$tid = $database->result($res, $i, "id");

				echo "<a href='/db/tournaments/?lookup=$tid'>$name</a>\n";
				echo "<br>\n";
			}
		endif;

		if ($fullview) :
			echo "</div>\n";
			echo "<div class='text'>\n";
		endif;

		include("hotstuff/.htconf");

		$hotstuff = new Database("postgresql");
		$hotstuff->connect($conf_host, $conf_name, $conf_user, $conf_pass);

		$res = $hotstuff->exec("SELECT * FROM directory WHERE author = '$this->realname' OR author = '$ggzuser'");
		if (($res) && ($hotstuff->numrows($res) > 0)) :
			echo "<h2>Game data</h2>\n";
			for ($i = 0; $i < $hotstuff->numrows($res); $i++)
			{
				$name = $hotstuff->result($res, $i, "name");

				echo "<a href='/hotstuff/'>$name</a>\n";
				echo "<br>\n";
			}
		endif;

		if ($fullview) :
			echo "</div>\n";
		endif;
	}

	function icon()
	{
		global $database;

		$ggzuser = $this->handle;

		$res = $database->exec("SELECT * FROM permissions WHERE handle = '$ggzuser'");
		if (($res) && ($database->numrows($res) > 0)) :
			$chat_bot = $database->result($res, 0, "chat_bot");
			$rooms_admin = $database->result($res, 0, "rooms_admin");
		endif;

		$pic = "player.png";
		if ($rooms_admin) :
			$pic = "admin.png";
		endif;
		if ($chat_bot) :
			$pic = "bot.png";
		endif;
		if ($this->handle == Auth::username()) :
			$pic = "you.png";
		endif;

		echo "<img src='/db/ggzicons/players/$pic' width=16 height=16>\n";
	}
}

?>
