<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");
include("database.php");
include("../admin/loginfunc.inc");
$global_leftbar = "disabled";
$global_rightbar = "disabled";

$checked = checkauthentication(default_realm());
$errorstr = $checked['errorstr'];
$authrequest = $checked['authrequest'];
$username = $checked['username'];

if ($authrequest) :
	request_authorization(default_realm());
endif;

include("top.inc");
if (!$errorstr) :
	// ok, valid username & password
	echo "<div id='adminmenu'>";
	echo "Community Administration";
	echo "</div>";
	echo "<br>";
	echo "<div id='main'>";
	echo "<div class='text'>";
	echo "Your are logged in as: " . $username . "<br>";
	echo "Proceed to the <a href='/admin/'>administration</a>.";
	echo "</div>";
	echo "</div>";
else :
	echo $errorstr;
endif;
include("bottom.inc");

?>
