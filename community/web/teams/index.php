<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

include("top.inc");

?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		Team management
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<div class="text">
	<a href="new.php">Create a new team</a>
	<br>
	<a href="join.php">Join or leave an existing team</a>
	</div>
	<h1>
		<span class="itemleader">:: </span>
		Your teams
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<div class="text">
	<a href="applications.php">Manage applications</a>
	<br>
	<a href="roles.php">Manage roles</a>
	</div>
</div>

<?php include("bottom.inc"); ?>
