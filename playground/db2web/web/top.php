<table border=0 cellspacing=0 cellpadding=1 width='100%'><tr><td bgcolor='#000000'>
<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#00ff00'>
<a href='index.php'><img src='ggzlogogreen.png' valign='middle' border=0></a>
&nbsp;&nbsp;
<font color='#000000' size=5>
GGZ Gaming Zone Ranking System
</font>
</td></tr></table>

</td></tr>
<tr><td bgcolor='#000000'>

<table border=0 cellspacing=0 cellpadding=5 width='100%'><tr><td bgcolor='#00ff00'>
<font color='#000000' size=2>

Statistics for GGZ version 0.0.7pre running on bizarre.mindx -
<?php
if ($ggzuser) :
	echo "Logged in as <b>$ggzuser</b>.\n";
else :
	echo "Not logged in\n";
	if (!$register) :
		echo "- <a href='index.php?register=1'>Register</a>\n";
	endif;
	echo ".\n";
endif;

echo "<table border=0 cellspacing=0 cellpadding=2><tr>\n";
if (!$register) :
	echo "<td bgcolor='#00c000'>\n";
else :
	echo "<td bgcolor='#ccc000'>\n";
endif;
if (!$register) :
	echo "<form action='account.php' method='POST'>\n";
else :
	echo "<form action='account.php?register=1' method='POST'>\n";
endif;
echo "<font color='#000000' size=2>\n";
if (!$ggzuser) :
	echo "Username: <input type='text' name='input_user' size=8>\n";
	echo "Password: <input type='password' name='input_pass' size=8>\n";
	if ($register) :
		echo "Realname: <input type='text' name='input_realname' size=8>\n";
		echo "Email: <input type='text' name='input_email' size=8>\n";
	endif;
	if ($register) :
		echo "&nbsp;<input type='submit' value='Register'>\n";
	else :
		echo "&nbsp;<input type='submit' value='Login'>\n";
	endif;
else :
	echo "<input type='submit' value='Logout'>\n";
endif;
echo "</font>\n";
echo "</form>\n";
echo "</td></tr></table>\n";

?>

</font>
</td></tr></table>

</td></tr></table>
<br><br>

<table border=0 width='100%'><tr><td valign=top>

