<?php

$datestamp = strtotime('now');
$date = getdate();

if (($_GET["day"] != null) && ($_GET["month"] != null) && ($_GET["year"] != null))
{
	$startdatestamp = strtotime($_GET["year"]."-".$_GET["month"]."-".$_GET["day"]);
	$startdate = getdate($startdatestamp);
}
else
{
	$startdatestamp = $datestamp;
	$startdate = $date;
}

$tomorrowdate = getdate(strtotime("tomorrow", $startdatestamp));
$yesterdaydate = getdate(strtotime("yesterday", $startdatestamp));

printf ("<table><tr><td><a href = 'show.php?year=%04d&month=%02d&day=%02d'>Previous Day</a></td>", $yesterdaydate['year'], $yesterdaydate['mon'], $yesterdaydate['mday']);

echo "<td><form action='index.php'>";
echo "<input type='hidden' name='page' value='$page'></input>";
echo '<select name="day">';

for ($day = 1; $day <= 31; $day++)
{
	if ($day == $startdate['mday'])
	{
		printf ("<option value='%02d' SELECTED>%02d</option>", $day, $day);
	}
	else
	{
		printf ("<option value='%02d' >%02d</option>", $day, $day);
	}
}

echo '</select>';
echo '<select name="month">';

for ($month = 1; $month <= 12; $month++)
{
	if ($month == $startdate['mon'])
	{
		printf ("<option value='%02d' SELECTED>%02d</option>", $month, $month);
	}
	else
	{
		printf ("<option value='%02d' >%02d</option>", $month, $month);
	}
}

echo '</select>';
echo '<select name="year">';



for ($year = $date['year']; $year <= $date['year'] + 5; $year++)
{
	if ($year == $startdate['year'])
	{
		echo "<option value='$year' SELECTED>$year</option>";
	}
	else
	{
		echo "<option value='$year' >$year</option>";
	}
}

echo "</select><input type='submit' value='Go'>";
echo "</form></td>";

printf ("<td><a href = 'show.php?year=%04d&month=%02d&day=%02d'>Next Day</a></td></tr></table>", $tomorrowdate['year'], $tomorrowdate['mon'], $tomorrowdate['mday']);

?>
 
