<?php $global_leftbar = "disabled"; ?>
<?php $global_rightbar = "disabled"; ?>
<?php include("top.inc"); ?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		GGZ Community Password Reset
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h1>
	<div class="text">
	If you have forgotten your password, it can be resent to you.
	This will however generate a new password, and will have to be
	activated by you.
	</div>
	<div class="text">
	<form class="leather" action="/login/" method="post">
		<h1>Resend password</h1>
		<h2>Email address</h2>
		<input type="text" size="20" maxlength="30" name="input_email">
		<br>
		<input type="submit" size="20" value="Resend" class="button">
	</form>
	</div>
</div>

<?php include("bottom.inc"); ?>
