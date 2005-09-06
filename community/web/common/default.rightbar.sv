<div id="rightbar">
	<h1>Nyheter</h1>

	<hr class="leather">

	<div class="menubar">
		<h1>Matcher/Turneringar</h1>
<?php
include_once("latest.php");
latest_matches();
latest_tournaments();
?>
	</div>

	<hr class="leather">

	<div class="menubar">
		<h1>Individuella bloggar</h1>
<?php
latest_blogs();
?>
	</div>

	<hr class="leather">

	<div class="menubar">
		<h1>Inlägg i forum</h1>
<?php
latest_forumposts();
?>
	</div>

	<hr class="leather">

	<div class="menubar">
		<h1>Players worldwide</h1>
<?php
include_once(Config::theme("mundomap.map"));
?>
		<a href="/map/"><img src="<?php Config::theme("mundomap.png"); ?>" usemap="#ggzmap" border="0"></a>
	</div>
</div>
