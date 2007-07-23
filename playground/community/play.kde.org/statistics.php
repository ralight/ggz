<?php
  $page_title = "Statistics";
  $site_root = ".";
  $site_menus = 2;
  include("header.inc");
?>

<h3>Live statistics - The top 10 in TicTacToe</h3>

<?php
//$url = "http://api.ggzcommunity/api/statistics/games/Reversi";
$url = "http://api.ggzcommunity/api/statistics/games/TicTacToe";

if (extension_loaded("curl")) :
	$tmphandle = tmpfile();
	$ch = curl_init();
	curl_setopt($ch, CURLOPT_URL, $url);
	curl_setopt($ch, CURLOPT_FILE, $tmphandle);
	$ret = curl_exec($ch);
	curl_close($ch);

	if ($ret) :
		fseek($tmphandle, 0);
		$xmlstr = fread($tmphandle, 16384);
		$doc = DomDocument::loadXML($xmlstr);
		$statistics = $doc->documentElement;
		$statsplayers = $statistics->childNodes;

		echo "<table>";
		echo "<tr>";
		echo "<th>Position #</th>";
		echo "<th>Player</th>";
		echo "<th>Score</th>";
		echo "</tr>";

		for ($i = 0; $i < $statsplayers->length; $i++)
		{
			$statsplayer = $statsplayers->item($i);
			$name = $statsplayer->getAttribute("name");
			$ratings = $statsplayer->childNodes;
			$rating = $ratings->item(0)->textContent;

			echo "<tr>";
			echo "<td>$i</td>";
			echo "<td>$name</td>";
			echo "<td>$rating</td>";
			echo "</tr>";
		}

		echo "</table>";

	else :
		echo "Oops! Error.";
	endif;

	fclose($tmphandle);
endif;

?>

<br/>

<?php
  include("footer.inc");
?>
