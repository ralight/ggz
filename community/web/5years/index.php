<?php $global_leftbar = "disabled"; ?>
<?php $global_rightbar = "disabled"; ?>
<?php include("top.inc"); ?>
<style type="text/css">
.article
	{
		margin: 12px;
		font-size: 6pt;
	}
tt
	{
		background-color: #90e090;
	}
</style>

<div id="main">
	<h1>
		<span class="itemleader">:: </span>
		Jubilee page: GGZ Gaming Zone turns Five!
		<span class="itemleader"> :: </span>
		<a name="playggzgames"></a>
	</h1>
	<div class="text">
	This page is dedicated to the 5th birthday of the GGZ Gaming Zone project.
	It contains funny quotes, old screenshots and some insight on how it all started...
	</div>

	<div class="text">
	On Monday, January 10 2000, the "Gnu Gaming Zone" project was founded on SourceForge.
	The <a href="http://sourceforge.net/projects/ggz/">project page</a> provided
	mailing lists, a CVS repository and (later on) homepage space to the project.
	GGZ was actually called "Gnu Gaming Zone" at that time, which led to some confusion...
	but read on to find out why :)
	<div class="article" style="float:left">
	<a href="netspades-console.png"><img src="thumb.netspades-console.png"></a>
	<br>
	NetSpades console client
	</div>
	Anyway, the project was founded when Brent M Hendricks, author of the
	<a href="http://www-ece.rice.edu/~brentmh/spades/">NetSpades</a>
	game, ran into Rich Gade, who was going to found a NetGames project.
	NetSpades at the time had a console client (screenshot on the left) and a Gtk+
	client. It's network code however was not very flexible, and integrating it
	into a general online gaming platform seemed to lead into the right direction.
	Once founded, GGZ quickly came forward with a Gtk+ client which was not tied
	to any game, but rather provided a graphical way to log into server and
	select a game to play.
	Brent then sent the following message to the
	<a href="https://mail.ggzgamingzone.org/cgi-bin/mailman/listinfo/ggz-dev">ggz-dev</a>
	mailing list:
	<div class="article" style="float:right">
	<a href="client-mockup.png"><img src="thumb.client-mockup.png"></a>
	<br>
	The first Gtk+ core client for GGZ
	</div>
	<tt>
	<br><br>
	<!-- bmh -->
	Agreed.  I'm attaching a screenshot (window-shot actually) of what I
	had for the new main window.  What do you think?
	<br><br>
	</tt>
	The design was generally accepted. Rich proposed to build a GNOME client
	using Glade, but back then this seemed to be too much overhead, so Brent
	replied:
	<tt>
	<br><br>
	<!-- bmh -->
	I use it [GNOME] at school, but my home computer is just too slow for it.  I'd like
	to have a strictly GTK client available for those who don't use GNOME.
	Should be as easy as a compile time option.
	<br>
	Also it would be great if we could get somebody to write a KDE client too.
	<br><br>
	</tt>
	The wish for the KDE client should turn into reality much faster than it was
	planned. One of GGZ's strengths, to provide gaming experience for various
	toolkits, was thus elaborated on by Rich in his follow-up mail.
	<tt>
	<br><br>
	<!-- rgade -->
	Agreed all around there, that's just what I was working on.  I'd also like to see
	Windows and Mac ports one day.
	<br><br>
	</tt>
	The project gained a bunch of developers really quickly. Doug Hudson was the first
	one to propose a project logo.
	<tt>
	<br><br>
	<!-- doug -->
	Here's a retry on the logo.  If it still doesn't come through right, let me
	know.
	</br><br>
	</tt>
	<div class="article" style="float:left">
	<a href="ggzlogo.png"><img src="thumb.ggzlogo.png"></a>
	<br>
	First try: GGZ project logo
	</div>
	A
	<a href="http://ftp.ggzgamingzone.org/pub/contrib/ggz_logo.png">different logo</a>
	was chosen after some discussion, which was rendered
	using Povray so it could be scaled up to arbitrary size.
	With all the development going at high pace, a first release was near,
	and so Brent sent a list of things still to do.
	<tt>
	<br><br>
	<!-- bmh -->
	Hey GGZ folks,
	<br>
	I'm having a slow day here, so I've finished adding the first round of
	chat support to the client and server.  So go ahead and 'cvs -z3
	update' your working directories.
	<br><br>
	I would really like to make an 0.0.1 release soon.  There are a few
	things which need done first.  Please remind me if I've forgotten
	anything.
	<br><br>
	- Convert README.Design to DocBook format (how is this going rich?)
	<br>
	- Update design docs to match design :)
	<br>
	- Write a HACKING file for people who want to delve into the code
	<br>
	- Write a README.game_modules for people interested in writing modules
	<br>
	- New website.  We need to decide on a logo, and get some content up
	  there.
	- Actual release of code on SourceForge (should be done before announcing)
	<br>
	- Announcements.  This may be the most important one.  We should have
	  a good announcement written up and then send it to SourceForge news,
	  freshmeat, linux game tome, linuxgames.com, etc.
	<br><br>
	</tt>
	Of course people would want to log into a server right away after downloading
	the client source, so Justin Zaun set up a
	<a href="http://www.ggzgamingzone.org/backend/server/">ggzd</a>
	at his computer.
	<tt>
	<br><br>
	<!-- jzaun -->
	Any deveoplers up for a chat? i have a GGZ server up at
	jzaun.static.telerama port 2222. See you there.
	<br>
	        -- Justin
	<br><br>
	</tt>
	It didn't take too long for the release to arrive.
	For the very first time, GGZ was unleashed upon the masses.
	Brent announced GGZ 0.0.1 on the mailing list:
	<tt>
	<br><br>
	<!-- bmh -->
	Subject: [ggz-dev] We did it!!
	<br>
	Version 0.0.1 of the Gnu Gaming Zone has been released!
	<br>
	I took the liberty of renaming our packages to ggz_client and ggz_server so
	that there will be less confusion (the executables are still named ggz and
	ggzd respectively).
	<br>
	<br>
	ggz_client-0.0.1.tar.gz
	<br>
	ggz_server-0.0.1.tar.gz
	<br>
	are bothe available from
	<br>
	ftp://download.sourceforge.net/pub/sourceforge/GGZ
	<br>
	http://download.sourceforge.net/GGZ
	<br>
	Most people on this list have cvs access, but spread the word!  I've
	submitted news on SourceForge, and first thing tomorrow monring I'll submit
	it to freshmeat.net as well.
	<br>
	We've got a long road ahead of us, but I think we've got a solid first
	release and great plans for the future.  Once the excitement settles down,
	we should start setting goals for 0.0.2. There's a tentative list in the
	TODO file, but we need to discuss them and then get to work.
	<br>
	Congratulations to all of us!
	<br><br>
	</tt>
	The release certainly didn't make the world spin faster, but nevertheless
	Justin could report the first success directly after the announcement
	was picked up by the gaming sites.
	<tt>
	<br><br>
	<!-- jzaun -->
	Subject: [ggz-dev] Download from SourceForge
	<br>
	We got 4 downloads of the server :-)
	<br><br>
	</tt>
	To which Brent replied:
	<tt>
	<br><br>
	<!-- bmh -->
	You should look at the IP address of the downloads.  At least one of
	them was probably me checking the links :)
	<br><br>
	</tt>
	Already back then, the
	<a href="http://freshmeat.net/projects/ggz">freshmeat entry</a>
	of GGZ brought a large number of users.
	<tt>
	<br><br>
	<!-- bmh -->
	Don't know if you guys saw this one.  I submitted GGZ on freshmeat
	this morning and it just showed up an hour ago.  Already we've had
	five downloads and 67 visits to our page.
	<br>
	Yay us!
	<br><br>
	</tt>
	A lot of bugs were found that had to be fixed, and of course the
	path to 0.0.2 required some new features as well.
	Rich declared his strategy on writing a config file parser to be:
	<tt>
	<br><br>
	<!-- rgade -->
	Got beer.
	<br><br>
	</tt>
	The most important discussion of that time however was which port
	number the GGZ server should use.
	Before the
	<a href="http://www.bekkoame.ne.jp/~s_ita/port/port5400-5999.html">formal assignment</a>
	could be done with IANA, the final candidate had to be elected,
	which Brent did on behalf of all others.
	<tt>
	<br><br>
	<!-- bmh -->
	5688 has some backing since we're using that as a public test server
	for released versions.  We could just make that the default, and then
	for testing development versions use some other number.
	<br><br>
	</tt>
	All of the above happened in the first three months!
	In April 2000, Brian Cox started the
	<a href="http://dev.ggzgamingzone.org/i18n/">GGZ internationalization</a>
	efforts by providing a German translation to the Gtk+ client.
	<tt>
	<br><br>
	<!-- bcox Apr 2000 -->
	The *ahem* long-awaited i18n enhancements have been committed.  It
	seems to work on my system.  Let me know how it works on yours.
	<br><br>
	</tt>
	One month later, Josef Spillner showed up for the first time,
	without yet really having a clue about multiplayer games.
	He was busy with his
	<a href="http://mindx.sourceforge.net/">MindX</a>
	project at that time, but would later fall in love with GGZ.
	<tt>
	<div class="article" style="float:right">
	<a href="ktictactux.png"><img src="thumb.ktictactux.png"></a>
	<br>
	KDE games module including KTicTacTux
	</div>
	<br><br>
	<!-- josef Jun 2000 -->
	Hello!
	<br>
	I would like to support the GGZ in my game Nautix (Sourceforge-ID:
	4279). Nautix is currently in a changing phase, I rewrote it from
	scratch to give strong multiplayer support. For now it's not a game but
	just a connection dialog, and I think it should provide different
	connection modes to be more flexible. I have already realized the
	connection procedure for the Zombie library
	(http://infa.abo.fi/~chakie/zombie/) but also want GGZ support.
	The difference between Nautix and the already supported games
	(TicTacToe, NetSpades) is that Nautix allowes changing the connection
	state during the game. You can play in single player mode, then connect
	to a server and play a multiplayer level. But it has also many common
	characteristics with the GGZ, for example the chat and the rooms (called
	Planets).
	So I want to ask if you have any suggestions how to realize the support.
	My idea is, independent from the GGZ main window, to have a room called
	"Nautix" where everyone playing my game can log in, chat and finally
	play with (or against) each other.
	Is it planned to make a Gaming Zone library one day which can easily be
	used for such things? Which features are intended for the future?
	<br>
	Thanks,
	<br>
	Josef Spillner
	dr_maux@users.sourceforge.net
	<br><br>
	</tt>
	As it turned out, providing GGZ support to external games was a
	much-wanted feature. Some weeks later, Dan Papasian joined and had a similar
	idea.
	<tt>
	<br><br>
	<!-- bugg Jul 2000 -->
	My name is Dan Papasian, and I'm the head
	developer of chessd.  http://chessd.sourceforge.net/
	<br>
	It'd be probably in the best interests of both
	of us if we figure out where we can work together
	with our projects.  First of all, let's get
	some issues out of the way.
	<br>
	1.  Licensing.
	...
	<br><br>
	</tt>
	A licensing debate followed, with the result that all libraries would
	be LGPL, and all applications would be GPL. However BSD code can be
	incorporated at any level, too.
	<br>
	After North America and Europe, it was time for South American developers
	to show up. Ismael Orenstein was the first one to do so.
	<tt>
	<br><br>
	<!-- perdig Sep 2000 -->
	Hello there!
	<br>
	My name is Ismael, and I'm from Rio de Janeiro (Brazil). I'm a big fan of
	Yahoo! Games, and I really liked the idea of building a similar, but 100%
	open, enviroment and wanted to contribute with it.
	<br><br>
	</tt>
	<div class="article" style="float:left">
	<a href="chess.png"><img src="thumb.chess.png"></a>
	<br>
	The very first GGZ chess client
	</div>
	He went on to get us a chess game using some of Dan's code. However not
	everyone was doing fine at the time, as a conversation between Rich and
	Brent showed:
	<tt>
	<br><br>
	<!-- rgade Sep 2000 -->
	On 6 Sep 2000, at 2:45, Brent M. Hendricks wrote:
	<br>
	&gt; PS.  Hmm, Doug's computer gets struck by lightnight, Justin deletes his
	<br>
	&gt; home dir, you and I get hard drive failures....perhaps we should start
	<br>
	&gt; warning our developers :)
	<br>
	Are you suggesting the project may be cursed?  Hehe, nah sounds too
	much like a bad TV show plot.
	<br><br>
	</tt>
	Of course, GGZ was not cursed. One important decision was still outstanding
	though. The project carried GNU in its name without being a GNU project,
	so we got in touch with
	<a href="http://www.stallman.org/">RMS</a>
	about the issue, who advised us in December 2000:
	<tt>
	<br><br>
	<!-- rms Dec 2000 -->
	Calling a program GNU software means that its developers and the GNU
	project agree that "This program is part of the GNU project, released
	under the aegis of GNU"--and say so in the program.
	<br><br>
	</tt>
	The GGZ developers could already foresee the project to grow to a size
	which allowed standing on its own, so a name change was due, which was
	proposed by Rich.
	<tt>
	<br><br>
	<!-- rgade Dec 2000 -->
	This is something which I've been considering for a while.  The initial
	desire was to be part of the GNU project, but as I stated I never heard
	back from them - and well, that kind of pissed me off and I never
	followed up on it.  I guess the logical name change should we decide
	not to join up with GNU would be "GGZ Gaming Zone" (so we don't have to
	change all the ggz references :)
	<br><br>
	</tt>
	That was the first year of GGZ, but in January 2001 new developer kept
	flowing in! Roger Light joined the project.
	<tt>
	<br><br>
	<!-- oojah Jan 2001 -->
	Hi,
	<br>
	I have been following GGZ for a fair while now, without being to
	contribute (no Linux install still). I do intend to get Linux running
	on my computer and get to grips with the GGZ source. However,
	I'm now behind a firewall that is very very strict. Only way out is
	via a proxy server. So my wish is for proxy server support in the
	clients. This will not be suitable for real time games and for other
	time dependant in some way, like Dan's chess 1 minute per move
	games, a warning of some type should be displayed. It would be
	very very useful though in my opinion.
	<br>
	Secondly, how far away is a 0.0.4 release?
	<br>
	And lastly... Is there any chance of a source release of the
	Windows client? Maybe and updated binary to be made available
	as well?
	<br>
	Thanks and keep up the good work,
	<br>
	Roger (oojah)
	<br><br>
	</tt>
	The mention of the windows port led to some more sophisticated build
	tools, so that after Roger got GGZ running under Cygwin, some parts
	even worked natively under Windows. But this only happened much later,
	so to go back to March 2001, yet another developer joined: Ricardo
	Calixto Quesada.
	<tt>
	<br><br>
	<!-- riq March 2001 -->
	Hi,
	I'm the author of TEG, a risk clone (http://teg.sourceforge.net).
	Justin told me about your project and I find it cool.
	Now I'm adding support for ggz to the server &amp; the client of TEG.
	<br>
	Well, thats all for now. As soon as I have something running I'll let you
	know.
	<br>
	riq.
	<br><br>
	</tt>
	His games TEG and Batalla Naval set new standards for the support of
	external games, and led to the development of
	<a href="http://www.ggzgamingzone.org/docs/api/ggzmod/">libggzmod</a>
	and
	<a href="http://www.ggzgamingzone.org/docs/api/ggzdmod/">libggzdmod</a>,
	the game support libraries.
	Even more interested people showed up, and although not everybody
	contributed code, their ideas are always appreciated. Rob Kaper, who
	like Josef is active in the
	<a href="http://games.kde.org/">KDE games</a>
	area, posted the following:
	<tt>
	<br><br>
	<!-- capsi Apr 2001 -->
	Hello,
	<br>
	I am developing a client/server model for the Monopoly boardgame. My initial
	intentions were to create my own server and a KDE client, but I might be
	interested in turning the application into a GGZ module or at least adding
	that capability as an option.
	<br>
	How far is GGZ regarding client/server communication etcetera? Would it be
	relatively easily to port an existing game engine to the GGZ environment?
	(at the moment the Monopoly server takes IRC-like commands, although I want
	to change that into a XML-like form)
	<br>
	Rob
	<br><br>
	</tt>
	And it still did not stop! In May 2001, Jason Dorje Short joined to
	create a multi-purpose card game, which led to many new features
	now available to other GGZ games as well.
	<tt>
	<br><br>
	<!-- jdorje May 2001 -->
	I want to write a game similar to NetSpades, but for a different
	Whist-based card game.  I've downloaded NetSpades 4.2, ggz 0.0.3, and
	the CVS version of ggz.  After looking at all of these, I have several
	questions/comments.
	<br><br>
	</tt>
	<div class="article" style="float:right">
	<a href="launch.png"><img src="thumb.launch.png"></a>
	<br>
	Gtk+ launch dialog
	</div>
	In May 2001, development went at high speed. A major change was
	proposed by Brent to move away from the old binary protocol.
	<tt>
	<br><br>
	<!-- bmh May 2001 -->
	What would everyone think about moving to a text/line oriented
	protocol (it would sure be easier to debug)?
	<br><br>
	</tt>
	And due to the high number of commits, a new mailing list was opened
	by Rich as well.
	<tt>
	<br><br>
	<!-- rgade May 2001 -->
	A lot of ggz-dev readers might not want to see the CVS commit log
	notifications, so a separate mailing list has been added for this.
	<br><br>
	</tt>
	Josef started working on some server-side tools, and started to
	operate another permanent GGZ server, this one being located in
	Germany.
	<tt>
	<br><br>
	<!-- josef Jun 2001 -->
	GGZ Europe One is connected to the internet, and I've got an account there.
	Justin, it's not games.snafu.de as I thought, but 213.73.91.24. I'll change
	this in the connection dialog and add it to KGGZ as well.
	<br><br>
	</tt>
	<div class="article" style="float:left">
	<a href="ggznow.png"><img src="ggznow.png"></a>
	<br>
	GGZ logo by Sebastian Roth
	</div>
	After some more releases, in November 2001 GGZ 0.0.5 was on the road.
	Another major improvement was the planned introduction of direct
	connections between game client and server, which so far was always
	proxied by the GGZ core client and server. Brent suggested:
	<tt>
	<br><br>
	<!-- bmh Nov 2001 -->
	Heh.  You left off your list the one thing that I would say actually
	has a chance of getting into 0.0.5: direct connections to the game
	client.  Of course, this probably doesn't require much (or any)
	modification from ggzdmod.  It's more on the ggzd side.  It will,
	however, require us to do more on the client side with ggzmod.
	<br><br>
	</tt>
	In Febrary 2002, a lengthy discussion about the feature of game
	spectators started. Jason's mail:
	<tt>
	<br><br>
	<!-- jdorje Feb 2002 -->
	Well, the same structure should work for spectators too.  But we haven't
	decided whether spectators will be in a separate list (which Brent and I
	want) or together in the same list (which Josef wants, and which will
	take less work) as human players.
	<br><br>
	</tt>
	At the end of 2002, GGZ 0.0.7 was going to be developed. Even though
	the homepage stayed at SourceForge, the switch to a new home was prepared
	by registering
	<a href="http://www.ggzgamingzone.org/">ggzgamingzone.org</a>
	and hosting the mailing lists there.
	Josef wrote:
	<tt>
	<br><br>
	<!-- josef Dec 2002 -->
	What about this plan for Xmas:
	<br>
	- register ggzgamingzone.org
	<br>
	- setup apache redirection to ggz.sf.net
	<br>
	- give developers email accounts <name>@ggzgamingzone.org
	<br>
	- register sub domains as aliases (CNAME) for our developer hosts
	<br>
	This makes us more independent, and it'd be a cool thing overall.
	<br><br>
	</tt>
	And one year later, in early 2004, GGZ left SourceForge. It does now
	have a new internationalized website, including mirrors, and
	along with celebrating the 5th birthday will soon publish the jubilee
	version 0.0.10.
	<br><br>
	Thanks for reading!
	</div>
</div>

<?php include("bottom.inc"); ?>
