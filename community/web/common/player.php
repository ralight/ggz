<?php

include_once("auth.php");

class Player
{
	var $realname;
	var $email;
	var $photo;
	var $gender;
	var $country;

	function Player($ggzuser)
	{
		global $id;

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
		echo "Gender: $this->gender<br>\n";
		echo "Country: $this->country<br>\n";
	}
}

?>
