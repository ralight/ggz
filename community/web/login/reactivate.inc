<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

$global_leftbar = "disabled";
$global_rightbar = "disabled";
include("top.inc");

?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		GGZ Community Password Reset
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<div class="text">
	The newly generated password can be activated now.
	</div>
	<div class="text">
	<form class="leather" action="/login/" method="post">
		<h2>Activate new password</h2>
		<br>
<?php
		$token = $_GET["token"];

		include_once("auth.php");
		Auth::checkactivation($token);
?>
	</form>
	</div>
</div>

<?php include("bottom.inc"); ?>
