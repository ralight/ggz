<?php

class Gender
{
	var $picsdir;
	var $gender;

	function Gender()
	{
		$docroot = $_SERVER["DOCUMENT_ROOT"];
		$this->picsdir = "$docroot/db/ggzicons/misc";
	}

	function load($gender)
	{
		$this->gender = $gender;
	}

	function name($gender)
	{
		return $gender;
	}

	function graphics($gender)
	{
		$pic = "$this->picsdir/$gender.png";
		if (file_exists($pic)) :
			return "<img src='/db/ggzicons/misc/$gender.png'>";
		endif;
		return "";
	}

	function listall()
	{
		$selected = "";
		if (!$this->gender) :
			$selected = " SELECTED";
		endif;
		echo "<option value=''$selected>(undisclosed)</option>\n";

		$selected = "";
		if ($this->gender == "male") :
			$selected = " SELECTED";
		endif;
		echo "<option value='male'$selected>male</option>\n";

		$selected = "";
		if ($this->gender == "female") :
			$selected = " SELECTED";
		endif;
		echo "<option value='female'$selected>female</option>\n";
	}	
}

?>
