<?php

$req = $_SERVER["REDIRECT_URL"];
$req = substr($req, 1, strlen($req) - 1);

if (($req == "docs") || ($req == "about") || ($req == "stats")) :
	$page = "$req.php";
	include($page);
else:
	$gamekey = $req;
	include("list.php");
endif;

?>
