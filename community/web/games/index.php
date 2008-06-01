<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

$global_leftbar = "disabled";
$global_rightbar = "disabled";
include("top.inc");

?>
<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		Play GGZ games
		<span class="itemleader"> :: </span>
		<a name="playggzgames"></a>
	</h1>
	<div class="text">
	In order to be able to play these games, you need at least a
	GGZ Gaming Zone core client and one or more games packages
	installed.
	If you're not sure about this, find out
	<a href="http://www.ggzgamingzone.org/">more information</a>.
	If you have Java installed, you can now also opt for playing
	GGZ games directly in the web browser! Just click the appropriate
	links below.
	<br><br>
	<a href="<?php echo Config::getvalue("ggzurl"); ?>">Launch external GGZ core client</a>
	</div>
	<h1>
		<span class="itemleader">:: </span>
		Play GGZ games with the GGZ-Java applet
		<span class="itemleader"> :: </span>
		<a name="room list"></a>
	</h1>
	<div class="text">
	Use this if you are on Windows to play on live.ggzgamingzone.org.
	<br><br>
	<!--
	<a href="applet.php?room=GGZCards%20-%20Whist">Whist</a>
	<br>
	<a href="applet.php?room=GGZCards%20-%20Bridge">Bridge</a>
	-->
	<a href="applet.php">Play with embedded client on the GGZ-Java page</a>
	</div>
	<h1>
		<span class="itemleader">:: </span>
		Complete room list
		<span class="itemleader"> :: </span>
		<a name="room list"></a>
	</h1>
	<div class="text">
	Use this if you are on Linux/BSD and have GGZ installed.
	You can find a dynamic view of all game rooms and running games on the
	<a href="http://metacle.ggzgamingzone.org/">Metacle</a>.
	<br><br>

<?php

if (Config::getvalue("ggzconfigfile")) :
	$rooms = $config_object->roomfilter;
	if (!$rooms) :
		// FIXME: we cannot easily access the database unless it contains a mapping to the filenames
		$roomarray = glob(dirname(Config::getvalue("ggzconfigfile")) . "/rooms/*.room");
	else :
		$roomarray = explode(" ", $rooms);
	endif;

	foreach ($roomarray as $room)
	{
		if ($room[0] != "/") :
			$roomconfigfile = dirname(Config::getvalue("ggzconfigfile")) . "/rooms/" . $room . ".room";
		else :
			$roomconfigfile = $room;
		endif;
		$ini = $config_object->parseconfigfile($roomconfigfile);
		if ($ini) :
			# FIXME: use i18n entries
			$roomname = $ini["RoomInfo"]["Name"];
			$url = Config::getvalue("ggzurl") . "/#" . htmlurl($roomname);
			echo "<a href='$url'>$roomname</a><br>\n";
			echo "<br>\n";
		else :
			echo "$room<br>\n";
			echo "<br>";
		endif;
	}
else :
	echo "No local GGZ installation found, no room listing possible.\n";
	echo "<br>\n";
endif;

?>

	</div>
</div>

<?php include("bottom.inc"); ?>
