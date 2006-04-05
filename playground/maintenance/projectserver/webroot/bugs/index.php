<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<title>
GGZ Gaming Zone Bug Tracker (bugs.ggzgamingzone.org)
</title>
</head>
<body bgcolor="#ffffff" text="#000000" link="#000000" alink="#ffffff" vlink="#999999">

<table border=0 cellspacing=0 width="100%"><tr>
<td bgcolor="#000000" width=40><img src="ggzlogo.png" alt="ggzlogo"></td>
<td bgcolor="#000000"><font color="#ffffff"><b>GGZ Gaming Zone Bug Tracker (bugs.ggzgamingzone.org)</b></font></td>
</tr></table>
<br><br>

<?php
if ($_SERVER['HTTPS'] != "on") :
	$host = $_SERVER['HTTP_HOST'];
	$requesturi = $_SERVER['REQUEST_URI'];
	echo "&gt;&gt; You might want to enable TLS for secure browsing.<br>\n";
	echo "&gt;&gt; <a href='https://$host$requesturi'>https://$host$requesturi</a><br>\n";
	echo "<br>\n";
endif;
?>

Our bug tracking system is implemented using Mantis, where you can report bugs or query exising ones.<br>
<a href="/mantis/">Mantis</a><br>
<br><br>

Bug reports are forwarded by Mantis to the
<a href="https://mail.ggzgamingzone.org/cgi-bin/mailman/listinfo/ggz-bugs">ggz-bugs</a>
mailing list, which is a read-only list
for developers to keep track of new bugs and status changes.
<br><br>

</body>
</html>
