<?php
  $page_title = "Play KDE games online";
  $site_root = ".";
  $site_menus = 2;
  include("header.inc");
?>

<h3>Join the GGZ Community</h3>
Several KDE games can be played on the GGZ Gaming Zone.
On the GGZ Community site, you'll find a lot of players who are waiting to play
against you. If there's nobody in the room, you'll still find a collection of
various computer players to match up against.
<a href="ggz://ggzcommunity.org">Play now</a>!
<br />

<h3>The games</h3>
The number of different games that can be played on the server is more than 20 already,
while a lot more game clients exist for KDE, GNOME/Gtk+, SDL and Java. The GGZ package
<a href="http://www.ggzgamingzone.org/packages/">ggz-kde-games</a> contains a few KDE
game clients. In addition, the KDE <a href="http://games.kde.org/">kdegames</a> package
is increasingly adding GGZ support. Therefore, the following KDE games can be played
on the server:
<br/>
KReversi, KBattleship (KDE 4)
<br/>
Fyrdman, KCC, KConnectX, KDots, Koenig, KGGZReversi, Krosswater, KTicTacTux, Muehle (KDE 3)
<br/>

<a href="images/kbattleship.png"><img src="images/thumb.kbattleship.png"></a>
<a href="images/kreversi.png"><img src="images/thumb.kreversi.png"></a>

<?php
  kde_general_news("news.rdf", 4, true);
?>

<br/>

<?php
  include("footer.inc");
?>
