<?php include_once("header.inc"); ?>

<?php

$res = pg_exec($conn, "SELECT * FROM games WHERE key = '$gamekey'");

$name = pg_result($res, 0, "name");
$logo = pg_result($res, 0, "logo");
$homepage = pg_result($res, 0, "homepage");
$zone = pg_result($res, 0, "zone");

if (!$logo) $logo = "none.png";

if ($zone) :
	$text = "gaming centre homepage";
else :
	$text = "game homepage";
endif;

?>

<div id="content">
	<h4>Game server listing</h4>
	<p>
		<a href="<?php echo $homepage; ?>"><?php echo $name; ?> <?php echo $text; ?></a>
		<img src="logos/<?php echo $logo; ?>">
		<br/><br/>
<?php
if ($zone) :
?>
		Game types which are playable in this centre are:
		<br/>
<?php
$res = pg_exec($conn, "SELECT * FROM zones WHERE key = '$gamekey'");
for ($i = 0; $i < pg_numrows($res); $i++)
{
	$key = pg_result($res, $i, "game");

	$res2 = pg_exec($conn, "SELECT * FROM games WHERE key = '$key'");
	for ($j = 0; $j < pg_numrows($res2); $j++)
	{
		$name2 = pg_result($res2, $j, "name");
		$logo2 = pg_result($res2, $j, "logo");
?>
		<img src="logos/<?php echo $logo2; ?>">
		<a href="/<?php echo $key; ?>"><?php echo $name2; ?></a>
		<br/>
<?
	}
}
?>
		<br/>
<?php
endif;
?>
		The following games are known to the metaserver of type
		<b><?php echo $name; ?></b>:
	</p>

	<span class="contentBox">
		Server URI: teg://teg.game-server.cc:2000<br/>
		Players: 4 (out of 5)<br/>
		Spectators: unknown<br/>
	</span>

</div>

<?php include_once("navigation.inc"); ?>

<?php include_once("footer.inc"); ?>
