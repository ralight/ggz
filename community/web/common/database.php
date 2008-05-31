<?php

class Database
{
	var $type;
	var $id;

	function Database($type)
	{
		if ($type == "pgsql") :
			$type = "postgresql";
		endif;

		$this->type = $type;
	}

	function connect($host, $name, $user, $pass, $port)
	{
		if ($this->type == "postgresql") :
			$this->id = @pg_connect("host=$host port=$port dbname=$name user=$user password=$pass");
		elseif ($this->type == "mysql") :
			$this->id = mysql_connect($host, $user, $pass);
			mysql_select_db($name, $this->id);
		endif;

		return $this->id;
	}

	function escape_string($string)
	{
		if ($this->type == "postgresql") :
			return pg_escape_string($string);
		elseif ($this->type == "mysql") :
			return mysql_real_escape_string($string, $this->id);
		endif;
	}

	function exec($query, $arguments)
	{
		/* $query should be a static string with no variables inside it.
		** To pass variables into the query, add %^ at the appropriate place
		** and add the variable to the $arguments array. e.g.
		**
		** $database->exec("SELECT * FROM users WHERE handle='%^' ORDER BY '%^';", array($handle, $order));
		**
		** This doesn't really offer any advantage over escaping everything
		** before the query is passed to exec() (ie. the person writing the
		** query still has to remember to pass the arguments so that they are
		** escaped whereas currently they just have to escape them manually)
		** but having to add the extra argument does make it less likely that
		** the escaping will be forgotten.
		*/

		$argcount = substr_count($query, "%^");
		if($argcount > 0){
			if(!is_array($arguments) || count($arguments) != $argcount){
				return NULL;
			}

			for($i = 0; $i < $argcount; $i++){
				$query = substr_replace($query, $this->escape_string($arguments[$i]), strpos($query, "%^"), 2);
			}
		}

		/* Query is now escaped correctly */
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
			return stripslashes(pg_result($result, $row, $column));
		elseif ($this->type == "mysql") :
			return stripslashes(mysql_result($result, $row, $column));
		endif;
	}
}

include_once("config.php");

$database = new Database(Config::getvalue("dbtype"));
$id = $database->connect(Config::getvalue("dbhost"), Config::getvalue("dbname"),
	Config::getvalue("dbuser"), Config::getvalue("dbpass"), Config::getvalue("dbport"));
if (!$id) :
	unset($database);
endif;

?>
