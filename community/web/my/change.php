<?php $global_leftbar = "disabled"; ?>
<?php include("top.inc"); ?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		Personal page (My Community): Change settings
		<span class="itemleader"> :: </span>
		<a name="personal"></a>
	</h1>
	<div class="text">
	Your personal settings can be done here.
	</div>
	<div class="text">

<?php

include_once("auth.php");
$ggzuser = Auth::username();

$res = pg_exec($id, "SELECT * FROM users WHERE handle = '$ggzuser'");
if (($res) && (pg_numrows($res) == 1)) :
	$realname = pg_result($res, 0, "name");
	$email = pg_result($res, 0, "email");
endif;
$res = pg_exec($id, "SELECT * FROM userinfo WHERE handle = '$ggzuser'");
if (($res) && (pg_numrows($res) == 1)) :
	$photo = pg_result($res, 0, "photo");
	$gender = pg_result($res, 0, "gender");
	$country = pg_result($res, 0, "country");
	$pubkey = pg_result($res, 0, "pubkey");
	$blogfeed = pg_result($res, 0, "blogfeed");
endif;

?>

<form action='settings.php?settings=1' method='POST'>
<table>
<tr><td>Real name:</td><td><input type='text' name='user_realname' value='<?php echo $realname; ?>'></td></tr>
<tr><td>Email address:</td><td><input type='text' name='user_email' value='<?php echo $email; ?>'></td></tr>
<tr><td>Photo:</td><td><input type='text' name='user_photo' value='<?php echo $photo; ?>'></td></tr>
<tr><td>Blog feed URL:</td><td><input type='text' name='user_blogfeed' value='<?php echo $blogfeed; ?>'></td></tr>
<tr><td>Gender:</td><td>
	<!-- <input type='text' name='user_gender' value='<?php echo $gender; ?>'> -->
	<select name='user_gender'>
<?php
include("genderlist.php");
$g = new Gender();
$g->load($gender);
$g->listall();
?>
	</select>
</td></tr>
<tr><td>Country:</td><td>
	<!-- <input type='text' name='user_country' value='<?php echo $country; ?>'> -->
	<select name='user_country'>
<?php
include("countrylist.php");
$c = new Country();
$c->load($country);
$c->listall();
?>
	</select>
</td></tr>
<tr><td></td><td><input type='submit' value='Change'></td></tr>
</table>
</form>

	</div>
	<div class="text">
	The password can be changed below.
	</div>
	<div class="text">

<form action='settings.php?password=1' method='POST'>
<table>
<tr><td>Password:</td><td><input type='password' name='user_password' value=''></td></tr>
<tr><td></td><td><input type='submit' value='Change'></td></tr>
</table>
</form>

	</div>
	<div class="text">
	For enhanced security, email communication can be encrypted using
	an OpenPGP key.
	Paste the output of 'gpg --export --armor 0xKEYIDNUM'.
	</div>
	<div class="text">

<form action='settings.php?pubkey=1' method='POST'>
<table>
<tr><td>Public key:</td><td><textarea name='user_pubkey' cols='40'><?php echo $pubkey; ?></textarea></td></tr>
<tr><td></td><td><input type='submit' value='Change'></td></tr>
</table>
</form>

	</div>
</div>

<?php include("bottom.inc"); ?>
