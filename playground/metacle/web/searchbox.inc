<div id="content">
	<h4>Search</h4>
	<form action="/search">
		<p>
		Find a specific game by entering a game name or category expression.
		You might also click on one of the links in the right navigation bar.
		<br/><br/>
			<span class="contentBox">
				Game:
				<input type="text" name="q" />
				<input type="submit" value="Search" class="button" />
			</span>
		</p>
	</form>
<?php
if ($q) :
?>
	<h4>Search results</h4>
	<p>
		The following games match the search term
		<b><?php echo $q; ?></b>:
		<br/>
<?php
$keys = array();

$res = pg_exec($conn, "SELECT * FROM games WHERE (lower(key) LIKE '%$q%') OR (lower(name) LIKE '%$q%')");
for ($i = 0; $i < pg_numrows($res); $i++)
{
	$key = pg_result($res, $i, "key");
	$keys[] = $key;
}

foreach ($keys as $key)
{
	$res = pg_exec($conn, "SELECT * FROM games WHERE key = '$key'");
	for ($i = 0; $i < pg_numrows($res); $i++)
	{
		$name = pg_result($res, $i, "name");
		$logo = pg_result($res, $i, "logo");

		if (!$logo) $logo = "none.png";
?>
		<img src="logos/<?php echo $logo; ?>">
		<a href="/<?php echo $key; ?>"><?php echo $name; ?></a>
		<br/>
<?
	}
}
?>
		<br/>
	</p>
<?php
endif;
?>
</div>
