<?php

$active = 0;
if (preg_match("/^\/tournaments\//", $_SERVER["REQUEST_URI"])) :
	$active = 1;
endif;
if (preg_match("/^\/teams\//", $_SERVER["REQUEST_URI"])) :
	$active = 1;
endif;
if (preg_match("/^\/my\//", $_SERVER["REQUEST_URI"])) :
	$active = 1;
endif;
if (preg_match("/^\/karma\//", $_SERVER["REQUEST_URI"])) :
	$active = 1;
endif;

if ($active) :
	include_once("auth.php");
	if(!Auth::username()) :
		echo "<div id='main'>";
		echo "<div class='text'>";
		echo __("You must be logged in to access this area.");
		echo "</div>";
		echo "</div>";
		include("bottom.inc");
		exit;
	endif;
endif;

?>
