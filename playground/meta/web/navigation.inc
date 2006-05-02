<?php

$zones = array();
$games = array();

$res = pg_exec($conn, "SELECT * FROM games");
for ($i = 0; $i < pg_numrows($res); $i++)
{
	$key = pg_result($res, $i, "key");
	$name = pg_result($res, $i, "name");
	$logo = pg_result($res, $i, "logo");
	$homepage = pg_result($res, $i, "homepage");
	$zone = pg_result($res, $i, "zone");

	if (!$logo) $logo = "none.png";

	$game = array();
	$game['name'] = $name;
	$game['logo'] = $logo;
	$game['homepage'] = $homepage;

	if ($zone) :
		$zones[$key] = $game;
	else :
		$games[$key] = $game;
	endif;
}

?>

<div id="rightNavigation">
	<div class="navBox">
		<i>Gaming Centres:</i>
<?php
foreach ($zones as $key => $game)
{
?>
		<img src="/logos/<?php echo $game['logo']; ?>">
		<a href="/<?php echo $key; ?>"><?php echo $game['name']; ?></a>
<?php
}
?>
	</div>
	<div class="navBox">
		<i>Individual games:</i>
<?php
foreach ($games as $key => $game)
{
?>
		<img src="/logos/<?php echo $game['logo']; ?>">
		<a href="/<?php echo $key; ?>"><?php echo $game['name']; ?></a>
<?php
}
?>
	</div>
</div>
