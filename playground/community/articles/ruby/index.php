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
		Article: GGZ and Ruby
		<span class="itemleader"> :: </span>
		<a name="articles"></a>
	</h1>
	<div class="text">
	13.11.2005 - written by <a href="mailto:josef@ggzgamingzone.org">Josef Spillner</a>
	<br><br>
	This article introduces the reader to the world of game server development for GGZ, based
	on what is already possible when trying to do it in <a href="http://www.ruby-lang.org/">Ruby</a>.
	It is targeted at developers, however a gamer might want to read up on how games come into
	existance.
	The game server <a href="http://www.ggzgamingzone.org/gameservers/rubytoe/">RubyToe</a>
	will be presented as a first result of the work being done.
	<br><br>
	The <a href="http://dev.ggzgamingzone.org/libraries/ggzdmod/">GGZdMod</a> library is used by
	more or less all game servers in GGZ. It is written in C, and therefore only useful per se
	for game servers written in C or C++. However, thanks to the <i>ggz-python</i> package, Python
	developers can also make use of it by means of using a wrapper, named <i>pyggzdmod</i>.
	A similar wrapper was planned for Ruby several years ago, and initial code was written, but
	only now did it happen that it is usable for full game server development.
	<br><br>
	Installation is possible by checking out the <a href="http://dev.ggzgamingzone.org/svn.php">Playground</a>
	module from SVN. Its directory <i>ggz-ruby/ruggzdmod</i> contains the wrapper file. Running
	<i>ruby extmod.rb</i> produces a makefile (if the values in extmod.rb are correct so it can
	find the ggzdmod library!), and then running make and make install will create and install
	the corresponding Ruby module.
	<br><br>
	A quick introduction on its usage is given here. The RubyToe server is referenced - note that
	many variable names and methods have been modified to be better understandable in the context
	of the article.
	First of all, the installed Ruby module needs to be included.
	<pre>
	require "GGZDMod"
	</pre>
	For real-world games, this statement should be packed into a begin/rescue statement, in order
	to allow conditional GGZ support.
	<pre>
	begin
	  require "GGZDMod"
	  $ggz = GGZDmod.new
	rescue LoadError
	  puts "Ouch, GGZ support is not possible."
	  $ggz = nil
	end
	</pre>
	The game server will likely consist of one or more class definitions, and some lines of GGZ
	initialisation code. For RubyToe, the classes are GGZSocket (emulating the functionality of
	<a href="http://dev.ggzgamingzone.org/libraries/libggz/">libggz</a>), TTTProto (containing the
	opcode definitions and logical send/receive operations), TTT (containing the game logic),
	and optionally the external TTTAI class to support a strong AI. A weak randomised AI is used
	in case the TTTAI module is not installed.
	<br><br>
	The initialisation code contains the callback handler for GGZ events, which must always be
	named <i>ggzdmod_handler</i>, the creation of the TTT object and entering the GGZ game server
	loop.
	<pre>
	def ggzdmod_handler(id, data)
	  $ttt.event(id, data)
	end

	$ttt = TTT.new
	$ggz.connect
	$ggz.loop
	</pre>
	The event method is called whenever a player joins, leaves or sends some data.
	The <i>id</i> constants are GGZDMod::EVENTJOIN, GGZDMod::EVENTLEAVE and GGZDMod::EVENTDATA, respectively.
	When a player joins, the game can start, assuming the other player is the AI; otherwise, the
	second player has to join as well. Whenever a player leaves before the game has ended, it
	should be paused to allow the player to rejoin. (Note however that RubyToe doesn't do this yet.)
	<br><br>
	Whenever data arrives, the <i>data</i> object contains the player number. Calling
	<pre>
	s = GGZSocket.for_fd($server.get_seat_fd(p))
	</pre>
	creates a socket for communication with the player in question. The socket can then be used
	to send and receive game-specific protocol messages. For turn-based games, the same sequence
	is used everywhere: read move, check move, apply move, broadcast move and find out next move.
	If this next move belongs to an AI player, invoke the AI and with its output restart the whole
	sequence, otherwise send the move request to the player.
	<br><br>
	That should suffice for a little introduction. An additional note shall be given on the AI usage
	in RubyToe. As mentioned, it can optionally support the external TTTAI module. This fits perfectly
	with GGZ's abilities to support <i>named bots</i>. Using RubyToe, just like when using the usual
	TicTacToe server, the TTTAI module can differentiate between an easy and a hard AI. In addition,
	RubyToe can provide a random AI player, which remains the only choice in case TTTAI is not installed.
	<br><br>
	Interested? Feel free to check out the sources and start to play with it. Somewhen in the future,
	a <i>ggz-ruby</i> package might be possible.
	</div>
</div>

<?php include("bottom.inc"); ?>
