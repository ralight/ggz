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

Statistics for GGZ version 0.0.7pre running on mindx.dyndns.org -
<?php
if ($ggzuser) :
	echo "Logged in as <b>$ggzuser</b>.\n";
else :
	echo "Not logged in.\n";
endif;

echo "<table border=0 cellspacing=0 cellpadding=2><tr><td bgcolor='#00c000'>\n";
echo "<form action='account.php' method='POST'>\n";
echo "<font color='#000000' size=2>\n";
if (!$ggzuser) :
	echo "Username: <input type='text' name='input_user' size=8>\n";
	echo "Password: <input type='password' name='input_pass' size=8>\n";
	echo "&nbsp;<input type='submit' value='Login'>\n";
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

