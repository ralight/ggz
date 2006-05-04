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
<body bgcolor="#dcdcdc" background="/pics/site/bg.png" text="#000000" link="#000000" vlink="#000000" alink="#000000">

<table class="pane" width="100%">
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

<table class="pane" width="100%">
<tr>
<td>
<font size="3">
<a href="/">Front page</a>
|
<a href="/software.php">Juegos y Applicaciones</a>
|
<a href="/developers.php">Desarrolladores</a>
|
<a href="/community.php">Comunidad</a>
|
<a href="/live.php">Juegos live</a>
</font>
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

	echo "<table class='pane' width='100%'>\n";
	echo "<tr>\n";
	echo "<td>\n";
	echo "<font size='3'>\n";
	echo "<font color='#ff0000'>Warning:</font>\n";
	echo "A newer version of this page is available in the original language.\n";
	echo "<a href='$file'>View latest version</a>.\n";
	echo "</font>\n";
	echo "</td>\n";
	echo "</tr>\n";
	echo "</table>\n";
}

?>

