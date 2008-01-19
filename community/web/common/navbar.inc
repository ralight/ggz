	<div class="menubar">
		<div class="menu">
			<a href="/" class="menuitem" title="_(Front page)">_(Main)</a>
		</div>
		<div class="menu">
			<a href="/db/" class="menuitem" title="_(Game statistics and rankings)">_(Database)</a>
			<div class="menu">
				<a href="/db/" class="menuitem">_(Overview)</a>
				<a href="/db/games/" class="menuitem">_(Games)</a>
				<a href="/db/matches/" class="menuitem">_(Matches)</a>
				<a href="/db/tournaments/" class="menuitem">_(Tournaments)</a>
				<a href="/db/players/" class="menuitem">_(Players)</a>
				<a href="/db/teams/" class="menuitem">_(Teams)</a>
			</div>
		</div>
<?php if ((Config::feature("ggzgames")) && (Config::feature("webgames"))) : ?>
		<div class="menu">
			<a href="/games/" class="menuitem" title="_(Some online fun)">_(Play!)</a>
			<div class="menu">
<?php if (Config::feature("ggzgames")) : ?>
				<a href="/games/" class="menuitem">_(GGZ games)</a>
<?php endif; ?>
<?php if (Config::feature("webgames")) : ?>
				<a href="/webgames/" class="menuitem">_(Web games)</a>
<?php endif; ?>
			</div>
		</div>
<?php endif; ?>
		<div class="menu">
			<a href="/active/hotstuff/" class="menuitem" title="_(Player contributions)">_(Manage!)</a>
			<div class="menu">
<?php if (Config::feature("datarepo")) : ?>
				<a href="/active/hotstuff/" class="menuitem">_(Game contents)</a>
<?php endif; ?>
				<a href="/active/tournaments/" class="menuitem">_(Tournaments)</a>
				<a href="/active/teams/" class="menuitem">_(Teams)</a>
<?php if (Config::feature("karma")) : ?>
				<a href="/active/karma/" class="menuitem">_(Karma)</a>
<?php endif; ?>
			</div>
		</div>
		<div class="menu">
			<a href="/forums/" class="menuitem" title="_(Forums)">_(Forums)</a>
			<div class="menu">
<?php if (Config::feature("forum")) : ?>
				<a href="/forums/" class="menuitem">_(Community forum)</a>
<?php endif; ?>
<?php if (Config::feature("blogs")) : ?>
				<a href="/blogs/" class="menuitem">_(Individual blogs)</a>
<?php endif; ?>
<?php if (Config::feature("articles")) : ?>
				<a href="/articles/" class="menuitem">_(Articles)</a>
<?php endif; ?>
			</div>
		</div>
		<div class="menu">
			<a href="/contact/" class="menuitem" title="_(About GGZ Community)">_(About)</a>
			<div class="menu">
				<a href="/contact/" class="menuitem">_(Contact)</a>
				<a href="/help/" class="menuitem">_(Help)</a>
			</div>
		</div>
<?php
// FIXME: better integration
//if ($_SERVER['PHP_AUTH_DIGEST']) :
include_once("player.php");
if (Auth::username()) :
	$p = new Player(Auth::username());
	if ($p->admin()) :
?>
		<div class="menu">
			<a href="/admin/" class="menuitem" style="background-color:#800000" title="_(Community Administration)">_(Admin)</a>
			<div class="menu">
				<a href="/admin/" class="menuitem">_(Admin Main)</a>
			</div>
		</div>
<?php
	endif;
endif;
?>
	</div>
