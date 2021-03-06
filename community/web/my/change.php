<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

$global_leftbar = "disabled";
include("top.inc");

?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		Personal page (My Community): Change settings
		<span class="itemleader"> :: </span>
		<a name="personal"></a>
	</h1>
	<div class="text">
	Your personal settings can be done here.
	To find out your coordinates (decimal longitude and latitude), you can check out
	<a href="http://www.getty.edu/research/conducting_research/vocabularies/tgn/index.html">TGN</a>
	or change them <a href="/map/change.php">interactively</a>.
	</div>
	<div class="text">

<?php

include_once("auth.php");
$ggzuser = Auth::username();

$res = $database->exec("SELECT * FROM users WHERE handle = '%^'", array($ggzuser));
if (($res) && ($database->numrows($res) == 1)) :
	$realname = $database->result($res, 0, "name");
	$email = $database->result($res, 0, "email");
endif;
$res = $database->exec("SELECT * FROM userinfo WHERE handle = '%^'", array($ggzuser));
if (($res) && ($database->numrows($res) == 1)) :
	$photo = $database->result($res, 0, "photo");
	$gender = $database->result($res, 0, "gender");
	$country = $database->result($res, 0, "country");
	$pubkey = $database->result($res, 0, "pubkey");
	$blogfeed = $database->result($res, 0, "blogfeed");
	$longitude = $database->result($res, 0, "longitude");
	$latitude = $database->result($res, 0, "latitude");
endif;

?>

<form action='settings.php?settings=1' method='POST'>
<table>
<tr><td>Real name:</td><td><input type='text' name='user_realname' value='<?php echo $realname; ?>'></td></tr>
<tr><td>Email address:</td><td><input type='text' name='user_email' value='<?php echo $email; ?>'></td></tr>
<tr><td>Photo:</td><td><input type='text' name='user_photo' value='<?php echo $photo; ?>'></td></tr>
<tr><td>Blog feed URL:</td><td><input type='text' name='user_blogfeed' value='<?php echo $blogfeed; ?>'></td></tr>
<tr><td>Gender:</td><td>
	<select name='user_gender'>
<?php
include_once("genderlist.php");
$g = new Gender();
$g->load($gender);
$g->listall();
?>
	</select>
</td></tr>
<tr><td>Country:</td><td>
	<select name='user_country'>
<?php
include_once("countrylist.php");
$c = new Country();
$c->load($country);
$c->listall();
?>
	</select>
</td></tr>
<tr><td>Longitude:</td><td><input type='text' name='user_longitude' value='<?php echo $longitude; ?>'></td></tr>
<tr><td>Latitude:</td><td><input type='text' name='user_latitude' value='<?php echo $latitude; ?>'></td></tr>
<tr><td></td><td><input type='submit' value='Change'></td></tr>
</table>
</form>

	</div>
	<div class="text">
	In case the language detection does not work or you prefer another language,
	this can be changed here.
	</div>
	<div class="text">

<form action='settings.php?language=1' method='POST'>
<table>
<tr><td>Language:</td><td>
<select name='user_language'>
<option value=''>(autodetect)</option>
<option value='de'>Deutsch</option>
<option value='en'>English</option>
<option value='fr'>Français</option>
<option value='es'>Español</option>
<option value='sv'>Svenska</option>
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
<tr><td>Public key:</td><td><textarea name='user_pubkey' cols='40' rows='5'><?php echo $pubkey; ?></textarea></td></tr>
<tr><td></td><td><input type='submit' value='Change'></td></tr>
</table>
</form>

	</div>
</div>

<?php include("bottom.inc"); ?>
