<?php

class Database
{
	var $type;
	var $id;

	function Database($type)
	{
		$this->type = $type;
	}

	function connect($host, $name, $user, $pass)
	{
		if ($this->type == "postgresql") :
			$this->id = pg_connect("host=$host dbname=$name user=$user password=$pass");
		elseif ($this->type == "mysql") :
			$this->id = mysql_connect($host, $user, $pass);
			mysql_select_db($name, $this->id);
		endif;
		return $this->id;
	}

	function exec($query)
	{
		if ($this->type == "postgresql") :
			return pg_exec($this->id, $query);
		elseif ($this->type == "mysql") :
			return mysql_query($query, $this->id);
		endif;
	}

	function numrows($result)
	{
		if ($this->type == "postgresql") :
			return pg_numrows($result);
		elseif ($this->type == "mysql") :
			return mysql_num_rows($result);
		endif;
	}

	function result($result, $row, $column)
	{
		if ($this->type == "postgresql") :
			return pg_result($result, $row, $column);
		elseif ($this->type == "mysql") :
			return mysql_result($result, $row, $column);
		endif;
	}
}

$database = new Database(Config::getvalue("dbtype"));
$database->connect(Config::getvalue("dbhost"), Config::getvalue("dbname"),
	Config::getvalue("dbuser"), Config::getvalue("dbpass"));

?>
