<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

?>
	<h2>
		<span class="itemleader">:: </span>
		List of available forums
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h2>
	<div class="text">
<?php
$res = $database->exec("SELECT * FROM forums", array());

for ($i = 0; $i < $database->numrows($res); $i++)
{
	$forumid = $database->result($res, $i, "id");
	$name = $database->result($res, $i, "name");
	$club = $database->result($res, $i, "club");

	echo "<a href='ggzforum.php?lookup=$forumid'>$name</a>\n";
	if ($club) :
		echo "(private club forum)";
	else :
		echo "(public forum)";
	endif;
	echo "<br>\n";
}
?>
	</div>
