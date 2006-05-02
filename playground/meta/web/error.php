<?php include_once("db.inc"); ?>
<?php

$req = $_SERVER["REDIRECT_URL"];
$req = substr($req, 1, strlen($req) - 1);

if (($req == "docs") || ($req == "about") || ($req == "stats") || ($req == "search")) :
	$page = "$req.php";
	include_once("header.inc");
	include($page);
else:
	if(strpos($req, "/")) :
		$ar = explode("/", $req);
		$gamekey = $ar[0];
		$format = $ar[1];
	else :
		$gamekey = $req;
		$format = "html";
	endif;

	$res = pg_exec($conn, "SELECT * FROM games WHERE key = '$gamekey'");
	if (pg_numrows($res) > 0) :
		if ($format == "html") :
			header("HTTP/1.1 200 Found");
			include_once("header.inc");
			include("list.php");
		elseif ($format == "rss") :
			header("HTTP/1.1 200 Found");
			include("rss.php");
		else :
			include_once("header.inc");
			include("unconfigured.inc");
			include("footer.inc");
		endif;
	else :
		include_once("header.inc");
		include("unconfigured.inc");
		include("footer.inc");
	endif;
endif;

?>
