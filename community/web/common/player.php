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
		global $id;

		$this->handle = $ggzuser;

		$res = pg_exec($id, "SELECT * FROM users WHERE handle = '$ggzuser'");
		if (($res) && (pg_numrows($res) == 1)) :
			$this->realname = pg_result($res, 0, "name");
			$this->email = pg_result($res, 0, "email");
		endif;
		$res = pg_exec($id, "SELECT * FROM userinfo WHERE handle = '$ggzuser'");
		if (($res) && (pg_numrows($res) == 1)) :
			$this->photo = pg_result($res, 0, "photo");
			$this->gender = pg_result($res, 0, "gender");
			$this->country = pg_result($res, 0, "country");
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
		echo "<br\n>";
		if ($this->country) :
			$c = new Country();
			$countryname = $c->name($this->country);
			$flag = $c->flag($this->country);
			echo "Country: $flag $countryname";
		else :
			echo "Country: (undisclosed)";
		endif;
		echo "<br\n>";
	}

	function items()
	{
		global $id;

		$ggzuser = $this->handle;

		$res = pg_exec($id, "SELECT teams.teamname, teams.fullname " .
			"FROM teammembers, teams " .
			"WHERE teams.teamname = teammembers.teamname AND handle = '$ggzuser'");
		if (($res) && (pg_numrows($res) > 0)) :
			echo "<h2>Teams</h2>\n";
			for ($i = 0; $i < pg_numrows($res); $i++)
			{
				$team = pg_result($res, $i, "teamname");
				$teamname = pg_result($res, $i, "fullname");
				if (!$teamname) :
					$teamname = "($team)";
				endif;
				echo "<a href='/db/teams/?lookup=$team'>$teamname</a>\n";
				echo "<br>\n";
			}
		endif;

		$res = pg_exec($id, "SELECT * FROM tournaments WHERE organizer = '$ggzuser'");
		if (($res) && (pg_numrows($res) > 0)) :
			echo "<h2>Tournaments</h2>\n";
			for ($i = 0; $i < pg_numrows($res); $i++)
			{
				$name = pg_result($res, $i, "name");
				$tid = pg_result($res, $i, "id");
				echo "<a href='/db/tournaments/?lookup=$tid'>$name</a>\n";
				echo "<br>\n";
			}
		endif;

		include("hotstuff/.htconf");

		$conn = pg_connect("host=$conf_host dbname=$conf_name user=$conf_user password=$conf_pass");

		$res = pg_exec($conn, "SELECT * FROM directory WHERE author = '$this->realname'");
		if (($res) && (pg_numrows($res) > 0)) :
			echo "<h2>Game data</h2>\n";
			for ($i = 0; $i < pg_numrows($res); $i++)
			{
				$name = pg_result($res, $i, "name");
				echo "<a href='/hotstuff/'>$name</a>\n";
				echo "<br>\n";
			}
		endif;
	}
}

?>
