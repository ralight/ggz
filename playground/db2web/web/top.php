<table border=0 cellspacing=0 cellpadding=1 width='100%'><tr><td bgcolor='#000000'>
<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#00ff00'>
<a href='index.php'><img src='ggzlogogreen.png' align='middle' border=0 alt="ggzlogo"></a>
&nbsp;&nbsp;
<font color='#000000' size=5>
GGZ Gaming Zone Ranking System
</font>
<!-- <br><font size=2>Attention: Statistics are not realtime yet!</font> -->
</td></tr></table>

</td></tr>
<tr><td bgcolor='#000000'>

<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#00ff00'>
<font color='#000000' size=2>

<?php
echo "Statistics for GGZ version $ggzversion running on $ggzname ($ggzhost) -\n";
if ($ggzuser) :
	echo "Logged in as <b>$ggzuser</b>.\n";
else :
	echo "Not logged in";
	if (!$register) :
		echo " - <a href='index.php?register=1'>Register</a>";
	endif;
	if (!$lostpw) :
		echo " - <a href='index.php?lostpw=1'>Lost password</a>";
	endif;
	echo ".\n";
endif;
echo "<br>\n";

echo "<map name='mymap'>";
echo "<table border=0 cellspacing=0 cellpadding=2><tr>\n";
if ($register) :
	echo "<td bgcolor='#ccc000'>\n";
elseif ($lostpw) :
	echo "<td bgcolor='#ccc000'>\n";
else :
	echo "<td bgcolor='#00c000'>\n";
endif;
if ($register) :
	echo "<form action='account.php?register=1' method='POST'>\n";
elseif ($lostpw) :
	echo "<form action='account.php?lostpw=1' method='POST'>\n";
else :
	echo "<form action='account.php' method='POST'>\n";
endif;
echo "<font color='#000000' size=2>\n";
if (!$ggzuser) :
	echo "Username: <input type='text' name='input_user' size=8>\n";
	if (!$lostpw) :
		echo "Password: <input type='password' name='input_pass' size=8>\n";
	endif;
	if ($register) :
		echo "Realname: <input type='text' name='input_realname' size=8>\n";
	endif;
	if (($register) || ($lostpw)) :
		echo "Email: <input type='text' name='input_email' size=8>\n";
	endif;
	if ($register) :
		echo "&nbsp;<input type='submit' value='Register'>\n";
	elseif ($lostpw) :
		echo "&nbsp;<input type='submit' value='Resend'>\n";
	else :
		echo "&nbsp;<input type='submit' value='Login'>\n";
	endif;
else :
	echo "<input type='submit' value='Logout'>\n";
endif;
echo "</font>\n";
echo "</form>\n";
echo "</td></tr></table>\n";

echo "</map>";

?>

</font>
</td></tr></table>

</td></tr></table>
<br><br>

<table border=0 width='100%'><tr><td valign=top>

