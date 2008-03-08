<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

$global_leftbar = "disabled";
$global_rightbar = "disabled";
include("top.inc");

?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		GGZ Community Registration
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<div class="text">
<?php if (Config::getvalue("registration") == "open") : ?>
	Everybody can register at <?php Config::put("name"); ?>.
<?php elseif (Config::getvalue("registration") == "closed") : ?>
	Registration at <?php Config::put("name"); ?> is not possible.
<?php elseif (Config::getvalue("registration") == "confirm") : ?>
	Everybody can register at <?php Config::put("name"); ?>.
	However, the account needs to be confirmed before it can be used.
<?php elseif (Config::getvalue("registration") == "confirmlater") : ?>
	Everybody can register at <?php Config::put("name"); ?>.
	Games can be played immediately after registration.
	However, the account must be confirmed within hours afterwards,
	otherwise it will be erased again.
<?php endif; ?>
	</div>

<?php if (Config::getvalue("registration") != "closed") : ?>
	<div class="text">
	Registration is not necessary, but gives you more power over the
	site: you'll be able to register tournaments, upload contents,
	and participate in forum discussion.
	Also, game rankings are based on this registration.
	Again: The GGZ server account, the GGZ Community account and the
	phpBB Forum account are all the same!
	</div>
	<div class="text">
	<form class="leather" action="/login/" method="post">
		<h1>Registration</h1>
		<h2>Username</h2>
		<input type="text" size="20" maxlength="15" name="input_user">
		<h2>Password</h2>
		<input type="password" size="20" maxlength="15" name="input_pass">
		<br>
		<h2>Email address</h2>
		<input type="text" size="20" maxlength="30" name="input_email">
		<br>
		<h2>Real name (optional)</h2>
		<input type="text" size="20" maxlength="30" name="input_realname">
		<br>
		<input type="submit" size="20" value="Register" class="button">
	</form>
	</div>
<?php endif; ?>
</div>

<?php include("bottom.inc"); ?>
