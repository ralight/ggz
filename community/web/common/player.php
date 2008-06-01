<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

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
	var $registered;

	function Player($ggzuser)
	{
		global $database;

		$this->handle = $ggzuser;

		$res = $database->exec("SELECT * FROM users WHERE handle = '%^'", array($ggzuser));
		if (($res) && ($database->numrows($res) == 1)) :
			$this->realname = $database->result($res, 0, "name");
			$this->email = $database->result($res, 0, "email");
			$this->registered = true;
		else :
			$this->registered = false;
		endif;
		$res = $database->exec("SELECT * FROM userinfo WHERE handle = '%^'", array($ggzuser));
		if (($res) && ($database->numrows($res) == 1)) :
			$this->photo = $database->result($res, 0, "photo");
			$this->gender = $database->result($res, 0, "gender");
			$this->country = $database->result($res, 0, "country");
		endif;
	}

	function genderref()
	{
		if ($this->gender == "male") :
			return "him";
		elseif ($this->gender == "female") :
			return "her";
		endif;
		return "him/her";
	}

	function display()
	{
		if ($this->photo) :
			echo "Photo:<br>\n";
			echo "<img src='" . html($this->photo) . "' height='64'>\n";
			echo "<br clear='all'>\n";
		else :
			echo "Photo: none found<br>\n";
		endif;
		echo "Real name: " . html($this->realname) . "<br>\n";
		echo "Email address: <a href='mailto:" . html($this->email) . "'>" . html($this->email) . "</a><br>\n";
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

		$res = $database->exec("SELECT * FROM permissionmasks WHERE handle = '%^'", array($ggzuser));
		if (($res) && ($database->numrows($res) > 0)) :
			$adminprivs = $database->result($res, 0, "admin_mask");
			$hostprivs = $database->result($res, 0, "host_mask");
			$anon = $database->result($res, 0, "anon_mask");
			$normal = $database->result($res, 0, "normal_mask");

			if ($adminprivs == "t") :
				$permission = "Administrator";
			elseif ($hostprivs == "t") :
				$permission = "Host";
			elseif ($normal == "t") :
				$permission = "Registered player";
			else :
				$permission = "Incomplete authority";
			endif;
		endif;

		echo "Permissions: $permission<br>\n";

		$res = $database->exec("SELECT * FROM permissions WHERE handle = '%^'", array($ggzuser));
		if (($res) && ($database->numrows($res) > 0)) :
			$join_table = $database->result($res, 0, "join_table");
			$launch_table = $database->result($res, 0, "launch_table");
			$rooms_login = $database->result($res, 0, "rooms_login");
			$rooms_admin = $database->result($res, 0, "rooms_admin");
			$chat_announce = $database->result($res, 0, "chat_announce");
			$chat_bot = $database->result($res, 0, "chat_bot");
			$no_stats = $database->result($res, 0, "no_stats");
			$edit_tables = $database->result($res, 0, "edit_tables");
			$table_privmsg = $database->result($res, 0, "table_privmsg");
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
		echo "Can send private messages at table: $table_privmsg<br>\n";
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
			"WHERE teams.teamname = teammembers.teamname AND handle = '%^' AND role <> ''", array($ggzuser));
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

		$res = $database->exec("SELECT * FROM tournaments WHERE organizer = '%^'", array($ggzuser));
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
		$hotstuff->connect($conf_host, $conf_name, $conf_user, $conf_pass, $conf_port);

		if ($hotstuff->id) :
			$res = $hotstuff->exec("SELECT * FROM directory WHERE author = '%^' OR author = '%^'", array($this->realname, $ggzuser));
			if (($res) && ($hotstuff->numrows($res) > 0)) :
				echo "<h2>Game data</h2>\n";
				for ($i = 0; $i < $hotstuff->numrows($res); $i++)
				{
					$name = $hotstuff->result($res, $i, "name");

					echo "<a href='/hotstuff/'>$name</a>\n";
					echo "<br>\n";
				}
			endif;
		else :
			echo "Game data repository not reachable.\n";
		endif;

		if ($fullview) :
			echo "</div>\n";
		endif;
	}

	function icon()
	{
		global $database;

		$ggzuser = $this->handle;

		$res = $database->exec("SELECT * FROM permissionmasks WHERE handle = '%^'", array($ggzuser));
		if (($res) && ($database->numrows($res) > 0)) :
			$botstatus = $database->result($res, 0, "bot_mask");
			$hostprivs = $database->result($res, 0, "host_mask");
			$adminprivs = $database->result($res, 0, "admin_mask");
		endif;

		if (strpos($this->handle, "|AI")) :
			$botstatus = 1;
		endif;

		$desc = "Guest";
		$pic = "guest.png";
		if ($botstatus == 't') :
			$pic = "bot.png";
			$desc = "AI/bot player";
		elseif ($this->registered) :
			$desc = "Player";
			$pic = "player.png";
			if ($adminprivs == 't') :
				$pic = "admin.png";
				$desc = "Administrator";
			elseif ($hostprivs == 't') :
				$pic = "host.png";
				$desc = "Host";
			endif;
			if (($this->handle) && ($this->handle == Auth::username())) :
				$pic = "you.png";
				$desc = "Yourself";
			endif;
		endif;

		echo "<img src='/db/ggzicons/players/$pic' width=16 height=16 title='$desc'>\n";
	}

	function admin()
	{
		global $database;

		$ggzuser = $this->handle;

		$res = $database->exec("SELECT * FROM permissionmasks WHERE handle = '%^'", array($ggzuser));
		if (($res) && ($database->numrows($res) == 1)) :
			$adminprivs = $database->result($res, 0, "admin_mask");

			if ($adminprivs == "t") :
				return true;
			endif;
		endif;

		return false;
	}

	function host()
	{
		global $database;

		$ggzuser = $this->handle;

		$res = $database->exec("SELECT * FROM permissionmasks WHERE handle = '%^'", array($ggzuser));
		if (($res) && ($database->numrows($res) == 1)) :
			$hostprivs = $database->result($res, 0, "host_mask");

			if ($hostprivs == "t") :
				return true;
			endif;
		endif;

		return false;
	}

	function exportxml()
	{
		$xml = new DOMDocument("1.0", "utf-8");
		$root = $xml->createElement("ggzplayer");
		$xml->appendChild($root);

		$handle = $xml->createElement("handle", $this->handle);
		$realname = $xml->createElement("realname", $this->realname);
		$email = $xml->createElement("email", $this->email);
		$photo = $xml->createElement("photo", $this->photo);
		$gender = $xml->createElement("gender", $this->gender);
		$country = $xml->createElement("country", $this->country);
		$root->appendChild($handle);
		$root->appendChild($realname);
		$root->appendChild($email);
		$root->appendChild($photo);
		$root->appendChild($gender);
		$root->appendChild($country);

		print $xml->saveXML();
	}

	function importxml($xmlfile)
	{
		$xml = new DOMDocument();
		$xml->load($xmlfile);
		$root = $xml->documentElement;
		if ($root->tagName != "ggzplayer") :
			return;
		endif;

		$elements = $root->getElementsByTagName("*");
		foreach ($elements as $element)
		{
			if ($element->tagName == "handle") :
				// ignore, because handle is fixed
			elseif ($element->tagName == "realname") :
				$this->realname = $element->nodeValue;
			elseif ($element->tagName == "email") :
				$this->email = $element->nodeValue;
			elseif ($element->tagName == "photo") :
				$this->photo = $element->nodeValue;
			elseif ($element->tagName == "gender") :
				$this->gender = $element->nodeValue;
			elseif ($element->tagName == "country") :
				$this->country = $element->nodeValue;
			else:
				// ignore unknown elements
			endif;
		}

		$this->savesettings();
	}

	function savesettings()
	{
		global $database;

		$res = $database->exec("UPDATE users SET email = '%^', name = '%^' WHERE handle = '%^'",
			array($this->email, $this->realname, $this->handle));
		$res = $database->exec("UPDATE userinfo SET photo = '%^', gender = '%^', country = '%^' " .
			"WHERE handle = '%^'",
			array($this->photo, $this->gender, $this->country, $this->handle));
	}
}

?>
