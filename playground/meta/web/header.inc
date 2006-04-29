<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
    <title>Metacle</title>
    <link rel="stylesheet" type="text/css" href="style.css"/>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
</head>

<body>
	<div id="container" >
		<div id="header">
			<h1>The Metacle</h1>
		</div>
		<div id="topNavigation">
			<ul>
				<li><a href="/about">About</a></li>
				<li><a href="/stats">Stats</a></li>
				<li><a href="/docs">Documentation</a></li>
				<li class="selected"><a href="/">Search</a></li>
			</ul>
		</div>

<?php

$ret = @include_once(".htconf");

if ($ret) :
	$conn = @pg_connect("host=$dbhost dbname=$dbname user=$dbuser password=$dbpass");
else :
	$conn = null;
endif;

if (!$conn) :
	include_once("unconfigured.inc");
	include_once("footer.inc");
	exit;
endif;

?>
