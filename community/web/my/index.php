<?php $global_leftbar = "disabled"; ?>
<?php include("top.inc"); ?>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		Personal page (My Community)
		<span class="itemleader"> :: </span>
		<a name="personal"></a>
	</h1>
	<div class="text">
	To modify the settings about you, please proceed to the
	<a href="change.php">change page</a>.
	</div>
	<div class="text">

<?php

include("auth.php");
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
endif;

?>

<?php

if ($photo) :
	echo "Photo:<br>\n";
	echo "<img src='$photo' width='100'>\n";
	echo "<br clear='all'>\n";
else :
	echo "Photo: none found<br>\n";
endif;
echo "Real name: $realname<br>\n";
echo "Email address: $email<br>\n";
echo "Gender: $gender<br>\n";
echo "Country: $country<br>\n";

?>

	</div>
	<div class="text">
	Tournaments I have organized:
	</div>
	<div class="text">
	...
	</div>
</div>

<?php include("bottom.inc"); ?>
