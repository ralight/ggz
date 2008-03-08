<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

$global_leftbar = "disabled";
include("top.inc");

?>

<?php

include_once("auth.php");
$ggzuser = Auth::username();

$res = $database->exec("SELECT * FROM userinfo WHERE handle = '%^'", array($ggzuser));
if (($res) && ($database->numrows($res) == 1)) :
	$request = $database->result($res, 0, "request");
endif;

?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		Personal page (My Community): Upgrade privileges
		<span class="itemleader"> :: </span>
		<a name="personal"></a>
	</h1>
	<div class="text">
	Seasoned players might apply for host privileges, i.e., getting a
	privilege cake in the cake bakery.
	Explain below why you want to apply and who of the current host
	players supports your cause.
	</div>
	<div class="text">

<?php if (!$request) : ?>

<form action='settings.php?applyforcake=1' method='POST'>
<table>
<tr><td>Reason:</td><td><textarea name='user_reason' cols='40' rows='5'></textarea></td></tr>
<tr><td></td><td><input type='submit' value='Apply'></td></tr>
</table>
</form>

<?php else : ?>

Your request is being worked on.

<?php endif; ?>

	</div>
</div>

<?php include("bottom.inc"); ?>
