<br>

<table class="pane" width="100%">
<tr>
<td>
<font size="3">
International
<?php

function country($lang)
{
	switch($lang)
	{
		case "en": return "gb"; break;
		default: return $lang;
	}
}

$dar = explode("/", $SCRIPT_FILENAME);
$i = 1;
while($dar[$i]) $i++;
$file = $dar[$i - 1];

$i = strlen($file);
while($file[$i] != ".") $i--;
$file = substr($file, 0, $i);

$dir = $SCRIPT_FILENAME;
$i = strlen($dir);
while($dir[$i] != "/") $i--;
$dir = substr($dir, 0, $i);

$d = opendir($dir);
while($f = readdir($d))
{
	if(strstr($f, "$file.")) :
		$far = explode(".", $f);
		$i = 0;
		while($far[$i]) $i++;
		$lang = $far[$i - 1];
		$country = country($lang);

		$img = "/pics/site/flags/flag-$country.png";
		echo "<a href=\"$file.$lang\"><img class=\"flag\" src=\"$img\" alt=\"$lang\"></a>\n";
	endif;
}
closedir($d);
?>
|
<a href="/about.php">About the GGZ Gaming Zone</a>
|
<a href="http://ggz.sourceforge.net">Old Homepage</a>
|
<a href="/contact.php">Contact</a>
</font>
</td>
</tr>
</table>

</body>
</html>

