<?php

$rssmax = -1;
if(strstr($_SERVER["PHP_SELF"], "/index.php")) :
	$rssmax = 3;
endif;
$rsscur = 0;

function xmlStart($parser, $name, $attributes)
{
	global $depth;
	global $entries;
	global $tagname;

	global $full;

	if ($name == "ITEM") :
		$full = "";
	endif;

	if ($name == "FULL") :
		$tagname = $name;
	endif;

	if ($name == "TITLE") :
		$tagname = $name;
	endif;

	if ($tagname != $name) :
		while (list($k, $v) = each($attributes))
		{
			$att .= " $k=$v";
		}
		$full .= "<$name$att>";
	endif;

	$depth[$parser]++;
}

function xmlEnd($parser, $name)
{
	global $depth;
	global $tagname;
	global $entries;

	global $allnews;
	global $full;

	global $rsscur;
	global $rssmax;

	$depth[$parser]--;

	if (($name == "ITEM") && ($rsscur != $rssmax)) :
		$allnews .= "$full<br><br>";
		$rsscur += 1;
	endif;

	if ($name == "FULL") :
		$tagname = "";
	endif;

	if ($name == "TITLE") :
		$tagname = "";
		$full .= "<br>";
	endif;

	if ($tagname != $name) :
		$full .= "</$name>";
	endif;
}

function xmlData($parser, $data)
{
	global $tagname;
	global $full;

	if ($tagname == "TITLE") :
		$full .= $data;
	endif;
	if ($tagname == "FULL") :
		$full .= $data;
	endif;
}

$rsslangs = array($lang, "en");
foreach ($rsslangs as $rsslang)
{
	$rss = "news.rss.$rsslang";
	if (file_exists($rss)) :
		break;
	endif;
}
$f = fopen($rss, "r");
$rssfeed = fread($f, filesize($rss));
fclose($f);

$xmlparser = xml_parser_create();
xml_set_element_handler($xmlparser, "xmlStart", "xmlEnd");
xml_set_character_data_handler($xmlparser, "xmlData");
xml_parse($xmlparser, $rssfeed);
xml_parser_free($xmlparser);

echo $allnews;

?>

