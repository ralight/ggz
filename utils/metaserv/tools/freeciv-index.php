<?php

header("Content-type: text/plain");

	$date = date("D M j H:m:s Y T");

	$query = "<?xml version=\"1.0\"?><query class=\"freeciv\" type=\"connection\">1.12.0</query>\n";

echo "Query: $query\n";
echo "Result:\n";
	$f = fsockopen("localhost", 15689, $errno, $errstr, 10);
	if($f) :
		fputs($f, $query);
		$allresult = "";
		while(!strstr($result, "\n"))
		{
			$result = fgets($f, 12);
			$allresult .= $result;
		}
		if(!$allresult) :
			$error = "No entries found.";
		endif;
		fclose($f);
	else :
		$error = "Couldn't connect to the meta server.";
	endif;

if($error) :
	echo "Error: $error\n";
endif;

echo $allresult;

function xmlStart($parser, $name, $attributes)
{
	global $depth;
	global $tagname;

	$tagname = $name;

	for($i = 0; $i < $depth[$parser]; $i++)
		echo "  ";
	print "$name\n";
	while(list($att, $value) = each($attributes))
	{
		echo " * $att = $value\n";
	}

	$depth[$parser]++;
}

function xmlEnd($parser, $name)
{
	global $depth;

	$depth[$parser]--;
}

function xmlData($parser, $data)
{
	global $tagname;

	echo "DATA: $data\n";

	if($tagname == "URI") :
		echo "Got URI!\n";
		$ar = explode(":", $data);
		$host = $ar[1];
		$host = substr($host, 2, strlen($host) - 2);
		$port = $ar[2];
		echo "Host: $host; Port: $port\n";
	endif;
}

$xmlparser = xml_parser_create();
xml_set_element_handler($xmlparser, "xmlStart", "xmlEnd");
xml_set_character_data_handler($xmlparser, "xmlData");
xml_parse($xmlparser, $allresult);
xml_parser_free($xmlparser);

exit;

?>
<!-- -->

<HTML><HEAD>
<META NAME="Author" CONTENT="GGZ Gaming Zone Meta Server">
<META NAME="Description" CONTENT="Freeciv servers">
<meta http-equiv="expires" content="0"><TITLE>Freeciv servers around the world.</TITLE></HEAD>
<BODY bgcolor=#FFFFFF>
<H1 align=center STYLE="font-family: 'ITC Zapf Chancery', 'Zapf Chancery'; font-size: 72pt">Freeciv</H1><center><table border=0 width=95%>
<tr><td bgcolor=#003364><FONT COLOR=#ffffff>Last update at: <?php echo $date; ?></FONT>
</td></tr>
</table></center>
<br>
<CENTER><TABLE BORDER=0 BGCOLOR="#FF6666" NOSAVE>
<TR><TD>Server</TD><TD>Port</TD><TD>Version</TD><TD>Status</TD><TD>Players</TD><TD>Server message</TD><TD>Last turn</TD><TD>Join as</TD></TR>

<TR BGCOLOR="#FFCC00" NOSAVE><TD><a href="freehtml0.html">civserver.freeciv.org</a></TD><TD>5550</TD><TD>1.12.0</TD><TD>Running</TD><TD>1</TD><TD>(game 29411) players welcome</TD><TD>2m</TD><TD>-</TD></TR>

<TR BGCOLOR="#33CCFF" NOSAVE><TD><a href="freehtml1.html">civserver.freeciv.org</a></TD><TD>5551</TD><TD>1.12.0</TD><TD>Pregame</TD><TD>0</TD><TD>(game 29442) players welcome</TD><TD>45m</TD><TD>-</TD></TR>

<?php

for($i = 0; $i < 20; $i++)
{
	if($i % 2) $color = "#33CCFF";
	else $color = "#FFCC00";

	echo "<TR BGCOLOR=\"$color\" NOSAVE><TD><a href=\"freehtml$i.html\">civserver.freeciv.org</a></TD>\n";
	echo "<TD>5550</TD><TD>1.12.0</TD><TD>Running</TD><TD>1</TD>\n";
	echo "<TD>(game 29411) players welcome</TD><TD>2m</TD><TD>-</TD></TR>\n";
}

?>

</TABLE>
&nbsp;
<br>
<br><br>
<center><table><tr><td width="400">Last turn shows time since server advanced a turn, 
useful if you want to find non-idle servers. 
Latest stable release is 1.12.0.  Please report any bugs to <a href="mailto:bugs@www.freeciv.org">bugs@www.freeciv.org</a><br><br>
From version 1.8.0, Freeciv servers doesn't automatically report to the metaserver, use the -m option, if you want your game listed.</td></tr></table></center></BODY>
</HTML>
