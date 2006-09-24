<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head><title>GGZ Schedule</title>
<link rel="stylesheet" href="style.css" type="text/css" />
<link rel="shortcut icon" href="/ggz_fav.png" />
</head>
<body>

<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");
include_once("auth.php");

date_default_timezone_set('US/Pacific');

if ($_GET["day"] && $_GET["month"] && $_GET["year"])
{
	$nowtime = strtotime($_GET["year"]."-".$_GET["month"]."-".$_GET["day"]);
}
else
{
	$datestring = date('y-m-d 00:00:00');
	$nowtime = strtotime($datestring);
}

$nowtimestring = date('d/m/y 00:00:00', $nowtime);
$nowdisplaystring = date('d/m/y', $nowtime);

if ($nowtime == strtotime(date('d/m/y 00:00:00')))
{
	echo "<h1>Today's empty tournaments spots ($nowdisplaystring)</h1>";
}
else
{
	echo "<h1>Empty tournaments spots for $nowdisplaystring</h1>";
}

?>

<table>
	<tr>
		<th>Date/Time</th>
	</tr>

<?php

$tomorrowtime = strtotime('tomorrow', $nowtime);
$yesterdaytime = strtotime('yesterday', $nowtime);
$week_day = date('l', $nowtime);

$res = $database->exec("SELECT date, time FROM manual_tournaments ".
	"WHERE date >= '$%' ".
	"AND date < '$%' ".
	"UNION SELECT start AS date, time FROM weekly_manual_tournaments ".
	"WHERE day = '%^' ".
	"AND start - time <= '%^' ".
	"AND (expire >= '%^' OR expire IS NULL) ORDER BY time",
	array($nowtime, $tomorrowtime, $week_day, $nowtime, $nowtime));

// Populate list with each possible slot in day, excluding taken slots
$currtime = $nowtime;
$i = 0;
$rowsnum = $database->numrows($res);

while ($currtime < $tomorrowtime)
{
	$showtime = true;

	if ($i < $rowsnum)
{
		$datetime = $database->result($res, $i, "date");
		if ($datetime == $currtime)
		{
			$i++;
			$showtime = false;
		}
	}

	if ($showtime == true)
	{
		$currtimestring = date('H:i e', $currtime);
		$currdate = getdate($currtime);
		printf ("<tr><td>$currtimestring</td><td><a href='index.php?page=new&year=%02d&month=%02d&day=%02d&hour=%02d&minutes=%02d'>Take Slot</a></td></tr>\n", $currdate['year'], $currdate['mon'], $currdate['mday'], $currdate['hours'], $currdate['minutes']);
	}

	$currtime = strtotime("+30 minutes", $currtime);
}

?>

</table>

<?php
	include("date-jump.inc");
	include("links.php");
?>

</body></html>
