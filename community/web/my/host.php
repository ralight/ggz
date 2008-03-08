<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

$global_leftbar = "disabled";
include("top.inc");

?>

<?php

if (!Auth::username()) :
	exit;
else:
	$ggzuser = Auth::username();
endif;

$p = new Player(Auth::username());
if (!$p->host()) :
	exit;
endif;

if ($_GET["givecake"]) :
	$player = $_GET["givecake"];
	$res = $database->exec("SELECT * FROM users WHERE handle = '%^'",
		array($player));
	if (($res) && ($database->numrows($res) == 1)) :
		$perms = $database->result($res, 0, "permissions");
		$perms += 256 + 128;
		$database->exec("UPDATE userinfo SET request = '' WHERE handle = '%^'",
			array($player));
		$database->exec("UPDATE users SET permissions = $perms WHERE handle = '%^'",
			array($player));
	endif;
endif;

?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		Personal page (My Community): Host player duties
		<span class="itemleader"> :: </span>
		<a name="personal"></a>
	</h1>
	<div class="text">
	As a host player, there are several functions available to you in addition to
	regular registered players and guest players.
	</div>

	<div class="text">
	The following players are applicants for host status.
	If you feel confident about their abilities, you can give them a 'cake'
	to give them host status, too.
	</div>

<?php

$res = $database->exec("SELECT * FROM userinfo WHERE request <> ''", array());
if (($res) && ($database->numrows($res) > 0)) :
for ($i = 0; $i < $database->numrows($res); $i++)
{
	$player = $database->result($res, $i, "handle");
	$reason = $database->result($res, $i, "request");
?>

	<div class="text">

<form action='host.php?givecake=<?php echo $player; ?>' method='POST'>
Player name: <?php echo $player; ?>
<br/>
Reason:
<br/>
<?php echo $reason; ?>
<br/>
<input type='submit' value='Accept'>
</form>

	</div>

<?php
}
endif;
?>

</div>

<?php include("bottom.inc"); ?>
