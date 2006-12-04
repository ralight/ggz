<html>
<head>
<title>
GGZ Gaming Zone
</title>
<?php
include("style.php");
?>
<meta http-equiv="Content-type" content="text/html; charset=utf-8">
<link rel="shortcut icon" href="/favicon.ico" type="image/x-icon">
</head>
<body>

<table class="pane">
<tr>
<td>
<font size="6">
<img src="/pics/site/ggzlogo.png" alt="GGZ logo" valign="middle">
&nbsp;&nbsp;
GGZ Gaming Zone
</font>
</td>
</tr>
</table>

<table class="pane">
<tr>
<td>
<a href="/">Front page</a>
|
<a href="/games/">Juegos</a>
|
<a href="/software/">Applicaciones</a>
|
<a href="/documentation/">Documentación</a>
|
<a href="/developers/">Desarrolladores</a>
|
<a href="/community/">Juegos live @ Comunidad GGZ</a>
</td>
</tr>
</table>

<br>

<?php

function outdated($lang = "en")
{
	$file = $_SERVER['REQUEST_URI'];
	if(substr($file, -4, 4) == ".php") $file = "$file.$lang";
	else $file = substr($file, 0, -2) . "$lang";

	echo "<table class='pane'>\n";
	echo "<tr>\n";
	echo "<td>\n";
	echo "<font color='#ff0000'>Warning:</font>\n";
	echo "A newer version of this page is available in the original language.\n";
	echo "<a href='$file'>View latest version</a>.\n";
	echo "</td>\n";
	echo "</tr>\n";
	echo "</table>\n";
}

?>

