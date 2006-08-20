<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");
include("top.inc");

echo "Administrative Commands<br>\n";
echo "<hr>\n";

include_once("database.php");

if ($_GET["check"] == "mail") :
	$email = Config::getvalue("mail");
	mail($email, "GGZ Community Admin: test mail", "");
	echo "Sent mail to $email<br>\n";
endif;

echo "<a href='index.php?check=mail'>test admin mail</a>";

include("bottom.inc")

?>
