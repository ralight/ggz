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
include_once("images/mundomap.map");
?>
		<a href="/map/"><img src="/images/mundomap.png" usemap="#ggzmap"></a>
	</div>
</div>
