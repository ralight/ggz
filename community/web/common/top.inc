<?php
include("locale.php");
Locale::detect();
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<html>
<head>
	<title>GGZ Community</title>
	<link href="/images/full.css" rel="stylesheet" type="text/css" title="All sidebars">
</head>

<body>
<?php include("navbar.php"); ?>

<div id="mainandacknowledgements">

<?php
if (preg_match("/^\/tournaments\//", $_SERVER["REQUEST_URI"])) :
	$active = 1;
endif;
if (preg_match("/^\/teams\//", $_SERVER["REQUEST_URI"])) :
	$active = 1;
endif;

if ($active) :
	include_once("auth.php");
	if(!Auth::username()) :
		echo "You must be logged in to access this area.";
		include("bottom.inc");
		exit;
	endif;
endif;
?>

