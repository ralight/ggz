<?php

class Live
{
	var $host;
	var $port;
	var $context;

	function Live($host, $port)
	{
		$this->host = $host;
		$this->port = $port;
	}

	function output()
	{
		$f = @fsockopen($this->host, $this->port, $errno, $errstr, 10);
		if ($f) :
			echo "The following people are currently playing games on $this->host:<br>\n";

			do
			{
				$tmp = fgets($f, 1024);
				$result .= $tmp;
			}
			while ($tmp);
			fclose($f);

			$xmlparser = xml_parser_create();
			xml_set_element_handler($xmlparser, "xmlStart", "xmlEnd");
			xml_set_character_data_handler($xmlparser, "xmlData");
			xml_parse($xmlparser, $result);
			xml_parser_free($xmlparser);
		else :
			echo "List of running games could not be determined.<br>\n";
		endif;
	}

	function xmlStart($parser, $name, $attributes)
	{
		$this->context = $name;

		if ($context == "GAME") :
			echo "Game of ", $attributes['NAME'], ":<br>\n";
		elseif ($context == "TABLE") :
			echo "* Table ", $attributes['ID'], ":<br>\n";
		endif;
	}

	function xmlEnd($parser, $name)
	{
		$this->context = "";
	}

	function xmlData($parser, $data)
	{
		if ($this->context == "SEAT") :
			echo "&nbsp;&nbsp;&nbsp;";
			echo "Seat: $data<br>\n";
		endif;
	}
}

?>

<?php
$live = new Live("localhost", 8080);

function xmlStart($parser, $name, $attributes)
{
	global $live;
	$live->xmlStart($parser, $name, $attributes);
}

function xmlEnd($parser, $name)
{
	global $live;
	$live->xmlEnd($parser, $name);
}

function xmlData($parser, $data)
{
	global $live;
	$live->xmlData($parser, $data);
}

?>

