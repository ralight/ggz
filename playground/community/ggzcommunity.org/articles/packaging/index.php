<?php $global_leftbar = "disabled"; ?>
<?php $global_rightbar = "disabled"; ?>
<?php include("top.inc"); ?>
<style type="text/css">
.article img
	{
		margin: 12px;
	}
</style>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		Article: GGZ Packaging
		<span class="itemleader"> :: </span>
		<a name="articles"></a>
	</h1>
	<div class="text">
	21.05.2005 - written by <a href="mailto:josef@ggzgamingzone.org">Josef Spillner</a>
	<br><br>
	GGZ is a huge project. It consists of many many packages, and tens of thousands
	of lines of code. Now it's not a huge monolithic project such as Mozilla or
	the Linux kernel, instead there are many small pieces which work together.
	Packagers and game authors can learn about the project step by step. In this
	article, it is shown which applications exist at all and in which packages
	they are located. Then, the individual
	<a href="http://www.ggzgamingzone.org/docs.php">documentation</a>
	can be consulted for more details for each of those.
	<div class="article" style="float:left">
	<img src="pkg-libggz.png" alt="libggz">
	</div>
	<br><br>
	The first such package is libggz, and it's an easy one, because it consists of only
	one library, which is (bingo!) libggz.
	By the way, the colour scheme is helping to differentiate the content a bit:
	the red entries are applications, the orange ones are plugins or applications
	not accessible to the user directly, and the yellow ones are shared libraries
	which might for instance be used by other game projects.
	<br>
	Now the task of libggz is to handle some basic data processing for the C
	programming language, mostly operations already present in higher-level languages,
	like for instance list creation, XML parsing or configuration file handling.
	It also offers simple networking and some basic security support through hashes
	and TLS/SSL encryption.
	<div class="article" style="float:right">
	<img src="pkg-ggzd.png" alt="ggzd">
	</div>
	<br><br>
	On the server side, there's the ggz-server package (named ggzd in CVS for
	historic reasons), which only depends on libggz and contains everything you
	need to run a GGZ server!
	Mainly this is the GGZ server itself (ggzd), an administration utility so
	the user database can be modified from the command line (ggzduedit),
	a library (ggzdmod) which is used for communication between the main server
	and the individual game servers, and then the whole lot of game servers.
	They're not all included in the diagram, but there are about two dozens
	of them. The GGZ Hosting Guide has more information about how to
	run a server.
	<div class="article" style="float:left">
	<img src="pkg-ggz-client-libs.png" alt="ggz-client-libs">
	</div>
	<br><br>
	Now let's go to the client side. Many client-side GGZ applications need
	a similar set of functions, therefore the package ggz-client-libs was
	created. It contains: ggzcore, used by core clients to communicate with
	the GGZ server, which means parsing and understanding the GGZ XML protocol;
	ggzmod, used by game clients to get all kind of information from the server;
	and two utility programmes: ggz-config maintains the game registry and
	ggz-wrapper acts as a minimal command line core client which automatically
	launches games if possible (and is used by the IM GGZ plugins, which will
	be introduced later).
	<div class="article" style="float:right">
	<img src="pkg-grubby.png" alt="grubby">
	</div>
	<br><br>
	Grubby is the GGZ chatbot. Technically, it is a client (and uses ggzcore),
	but in reality it runs on a server so it is always available. Beside
	grubby itself, the package contains the configuration script grubby-config,
	which can be used to configure the chatbot on the command line. A graphical
	configuration is also possible and will be introduced later.
	There are many plugins and core plugins as part of grubby, and several
	small scripts. The Grubby Architecture document has the details.
	<div class="article" style="float:left">
	<img src="pkg-utils.png" alt="utils">
	</div>
	<br><br>
	The GGZ utilities package (commonly called utils) contains several small
	tools useful mainly for administrators and developers. The cmd-client,
	or ggz-cmd, can broadcast messages to all rooms of a GGZ server and
	also monitor it for outages and the like. With commgen, protocol handling
	network source code can be created for games out of XML descriptions,
	which is currently used for the Tic-Tac-Toe game server.
	Metaserv is the GGZ metaserver, which can store arbitrary tree-structured
	data, and can be used to get information about where GGZ servers are
	running. TelGGZ is a little telnet wrapper which is useful to access
	GGZ server without a proper core client, e.g. from a cybercafé.
	<div class="article" style="float:right">
	<img src="pkg-txt-client.png" alt="txt-client">
	</div>
	<br><br>
	Still reading? Good. The following applications are the already
	named core clients, that is, chat clients to access GGZ servers
	with the ability to launch, join or watch games. There are five of
	them as part of GGZ, and the first one is ggz-txt, a very minimalistic
	client mainly to be used on the command line.
	In graphical environments it can however also launch games of any
	kind, including fullscreen ones. The single other piece of software
	in the txt-client package is tttxt, an also minimalistic version
	of the tic-tac-toe game. This is the only GGZ game with a text
	interface so far.
	<div class="article" style="float:left">
	<img src="pkg-gtk-client.png" alt="gtk-client">
	</div>
	<br><br>
    The second one, ggz-gtk, is the core client
	for the Gtk+ toolkit. It would integrate nicely into a XFCE desktop,
	and the package also contains the Gaim plugin, so Gaim users
	can play GGZ from within their IM application.
	Along with the following KGGZ, ggz-gtk is one of the two "advanced"
	core clients which offer everything currently available on GGZ,
	including player lag meter, public and private and table chat,
	recognition of registered players, bots and admins, and much more.
	<div class="article" style="float:right">
	<img src="pkg-kde-client.png" alt="kde-client">
	</div>
	<br><br>
	For KDE, a core client (KGGZ) also exists, but the respective package
	does not only contain KGGZ. There is GGZap, a special core client
	which lives in the taskbar tray and will only wake up if others
	join a room to play, so there isn't much distraction. (Besides, KGGZ
	does now offer a similar notification scheme.) The red gear of GGZap
	makes it a very unique application.
	Then there's KGrubby, the already promised graphical configuration
	for the chatbot grubby. Furthermore, compcal (and event/competition
	calendar), keepalivecontrol (to log into running game servers and
	administrate them) and shadowbridge (to intercept and inspect game
	messages) exist.
	<br>
	As if this wouldn't be enough yet, the kde-client package comes
	with a KDE control center module (kcm_ggz), a KIO slave to access
	GGZ servers (kio_ggz) and another KIO slave to get information
	from metaservers of many projects, including GGZ (of course),
	Freeciv, Atlantik and so on.
	<div class="article" style="float:left">
	<img src="pkg-gnome-client.png" alt="gnome-client">
	</div>
	<br><br>
	There is also a GNOME client, which is rather geared
	towards people without much GGZ experience. It is very easy to
	use, but on the other hand doesn't offer much settings.
	Along with it, the MOTD editor is shipped, which is a
	pure GNOME application to edit MOTD messages. Such messages
	are used by the GGZ server to inform the user about
	whatever needs to be informed about upon login, unless
	the client and server agree to use a web-based MOTD.
	The text MOTDs can be coloured, and this is what the MOTD editor
	is for.
	<div class="article" style="float:right">
	<img src="pkg-gtk-games.png" alt="gtk-games">
	</div>
	<br><br>
	That was it for the core clients, but what is a game project without
	games? GGZ has four game packages, each of them groups games with
	the same toolkit together. Note that games of all toolkits can be
	used independent of the core client implementation!
	The first such group is the gtk-games package, which has about
	a dozen games available. All of these games require GGZ to be played,
	that means they do not run in single-player mode.
	<div class="article" style="float:left">
	<img src="pkg-sdl-games.png" alt="sdl-games">
	</div>
	The same is true for sdl-games, the next group. Only two games are
	included here, but both run full-screen and offer more graphics,
	with one of the (ttt3d) even running using OpenGL graphics.
	<br><br>
	<div class="article" style="float:right">
	<img src="pkg-kde-games.png" alt="kde-games">
	</div>
	<br><br>
	There's also a hand full of KDE games, and many of those provide
	a local game mode against the AI also, even though playing on GGZ is
	much more fun of course. Many of the games are reimplementations of their
	Gtk+ counterparts, so people who have installed both packages can choose
	which fits them better.
	<div class="article" style="float:left">
	<img src="pkg-python.png" alt="python">
	</div>
	<br><br>
	The last and probably most complicated package is the one which contains
	all the python development. This comprises game clients, game servers
	and a core client.
	The client (víbora) is a full-screen client with many graphical elements,
	but rather basic functionality. The games differ in their implementation,
	the most advanced one is certainly GGZBoard, which under one hood offers
	five games for local or GGZ game, and an additional one (Ludo) for local
	play. To save development time, all of the base GGZ libraries are reused
	by the python applications, by means of wrappers.
	<br><br>
	There's much more to GGZ actually - there's an internationalised WWW
	module where the web pages are hosted, there's the GGZ Community which
	is mostly implemented in PHP, there's the Playground where new projects
	incubate and there's the ever-growing documentation package ggz-docs.
	If you're a distributor, do not hesitate to package all of GGZ, package
	after package. It isn't that hard and it will make it much easier in the
	future to enable the GGZ support in all the other games which support GGZ!
	</div>
</div>

<?php include("bottom.inc"); ?>
