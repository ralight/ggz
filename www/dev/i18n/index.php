<? include_once("../site/header.php"); ?>

<table class="content" width="100%">
<tr>
<td>
<font size="3">

<b>GGZ Internationalization</b><br>
Both the software and the document files (web pages, man pages, HTML docs) adhere to high
standards of internationalization. This includes the requirement of gettext as well as
per-user languages in multi-user environments such as the web pages or the grubby output.
<br>

<table class="inner">
<tr>
<td valign="top" colspan="2">

<i>Translatable texts</i><br>
A script automatically assembles all texts which are translatable into a catalog.
Downloading the potfile catalog is a convenient way of starting a new translation.
Webpage translations are easily done by checking out the 'www' module from
<a href="http://cvs.ggzgamingzone.org/">GGZ CVS</a> and adding files ending in .php.$lang.
<br>
<a href="ggzpots.tar.gz">Download the potfile catalog!</a>

<br><br>

<i>Guidelines</i><br>
The internationalization guidelines are available in the ggz-docs package.
<br>
<a href="http://cvs.ggzgamingzone.org/cgi-bin/viewcvs.cgi/docs/ggz-project/Internationalization?rev=HEAD&content-type=text/vnd.viewcvs-markup">Current version</a>

<br><br>

<i>PO statistics</i><br>

<?php

$stats = array();
$languages = array();
$modules = array();
$module = "unknown";
$repeat = 0;

$f = fopen("postats.txt", "r");
while($a = fgets($f, 1024))
{
	if (preg_match("/(\S+)\.po\ +\-\ +(\d+)\/(\d+)/", $a, $matches)) :
		$lang = $matches[1];
		$number = $matches[2];
		$total = $matches[3];
		$stats[$module][$lang] += $number;
		$stats[""][$lang] += $number;
		$languages[$lang] = 1;
		if($repeat) :
			$stats[$module][""] += $total;
			$stats[""][""] += $total;
			$repeat = 0;
		endif;
	else :
		$modulear = explode("/", $a);
		$module = $modulear[0];
		$repeat = 1;
		$modules[$module] = 1;
	endif;
}
fclose($f);

?>

<table class="inner">
<tr>
<td valign="top" colspan="2">

<i>Module list</i><br>

<table class="list">

<?php

foreach($stats as $module => $modulear)
{
	if ($module == "") continue;

	foreach($languages as $lang => $foo)
	{
		if (!$stats[$module][$lang]) :
			$stats[$module][$lang] = 0;
		endif;
	}
}

$modules[""] = 1;
foreach($modules as $module => $foo)
{
	$modulename = $module;
	if ($module == "") :
		$modulename = "TOTAL";
	endif;

	echo "<tr><td>$modulename</td>";
	$langnum = $stats[$module][""];
	echo "<td>TOTAL $langnum</td>";

	foreach($languages as $lang => $foo)
	{
		if ($lang == "") continue;
		$langnum = $stats[$module][$lang];
		if ($langnum) :
			echo "<td>$lang $langnum</td>";
		else :
			echo "<td></td>";
		endif;
	}

	echo "</tr>";
}

?>

</table>

</td>
</tr>
</table>

<br>

<i>HTML statistics</i><br>

<br><br>

<i>Manpage statistics</i><br>

<br><br>

</td>
</tr>
</table>

</font>
</td>
</tr>
</table>

<? include_once("../site/footer.php"); ?>

