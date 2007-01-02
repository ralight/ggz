<?php

class GGZD
{
	var $url;
	var $host;
	var $port;

	var $id;
	var $name;
	var $version;
	var $status;
	var $tlssupport;
	var $chatlength;

	var $error;

	function GGZD($url)
	{
		$this->url = $url;

		$urlparts = parse_url($url);
		$this->host = $urlparts['host'];
		$this->port = $urlparts['port'];
	}

	function parse()
	{
		$f = @fsockopen($this->host, $this->port, $errno, $errstr, 10);
		if ($f) :
			$this->error = 0;

			stream_set_timeout($f, 0, 20000);
			do
			{
				$tmp = fgets($f, 1024);
				$result .= $tmp;
			}
			while (strlen($tmp) > 0);
			fclose($f);

			$xmlparser = xml_parser_create();
			xml_set_element_handler($xmlparser, "xmlStart", "xmlEnd");
			xml_set_character_data_handler($xmlparser, "xmlData");
			xml_parse($xmlparser, $result);
			xml_parser_free($xmlparser);
		else :
			$this->error = 1;
		endif;
	}

	function output()
	{
		if (!$this->error) :
			echo "GGZ server on $this->host claims to be:<br>\n";

			echo "GGZ Gaming Zone protocol version <b>$this->version</b><br>";
			echo "Server status <b>$this->status</b><br>";
			echo "Server name <b>$this->name</b><br>";
			echo "Server identifier <b>$this->id</b><br>";
			echo "Server URI <b><a href='$this->url'>$this->url</a></b><br>";
			echo "Allows encrypted connections: <b>$this->tlssupport</b><br>";
			echo "Allows chat length: <b>$this->chatlength characters</b><br>";
		else :
			echo "Connection to the GGZ server failed.<br>\n";
		endif;
	}

	function xmlStart($parser, $name, $attributes)
	{
		if ($name == "SERVER") :
			if ($attributes['ID']) :
				$this->id = $attributes['ID'];
			endif;
			if ($attributes['NAME']) :
				$this->name = $attributes['NAME'];
			endif;
			if ($attributes['VERSION']) :
				$this->version = $attributes['VERSION'];
			endif;
			if ($attributes['STATUS']) :
				$this->status = $attributes['STATUS'];
			endif;
			if ($attributes['TLS_SUPPORT']) :
				$this->tlssupport = $attributes['TLS_SUPPORT'];
			endif;
		elseif ($name == "OPTIONS") :
			if ($attributes['CHATLEN']) :
				$this->chatlength = $attributes['CHATLEN'];
			endif;
		endif;
	}

	function xmlEnd($parser, $name)
	{
	}

	function xmlData($parser, $data)
	{
	}
}

?>

<?php

function xmlStart($parser, $name, $attributes)
{
	global $ggzd;
	$ggzd->xmlStart($parser, $name, $attributes);
}

function xmlEnd($parser, $name)
{
	global $ggzd;
	$ggzd->xmlEnd($parser, $name);
}

function xmlData($parser, $data)
{
	global $ggzd;
	$ggzd->xmlData($parser, $data);
}

?>

