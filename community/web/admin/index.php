<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");
$global_leftbar = "disabled";
$global_rightbar = "disabled";
include("top.inc");

echo "Administrative Commands<br>\n";
echo "<hr>\n";

include_once("database.php");

if ($_GET["check"] == "mail") :
	$email = Config::getvalue("mail");
	mail($email, "GGZ Community Admin: test mail", "");
	echo "Sent mail to $email<br>\n";
elseif ($_GET["check"] == "phpinfo") :
	phpinfo();
else:
	echo "<a href='index.php?check=mail'>test admin mail</a><br>";
	echo "<a href='index.php?check=phpinfo'>phpinfo output</a><br>";
endif;

include("bottom.inc")

?>
