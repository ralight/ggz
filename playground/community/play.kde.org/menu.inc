<?php
$this->setName("Landing page");

$section =& $this->appendSection("Inform");
$section->appendLink("Home","");

$section->appendLink("News","news.php");
$section->appendLink("How to play","howto.php");
$section->appendLink("Statistics","statistics.php");

$section =& $this->appendSection("Links");
$section->appendLink("KDE Games Home","http://games.kde.org/",false);
$section->appendLink("GGZ Community","http://www.ggzcommunity.org/",false);
$section->appendLink("GGZ Project","http://www.ggzgamingzone.org/",false);
?>
