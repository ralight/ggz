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
		Article: Introduction to GGZBoard
		<span class="itemleader"> :: </span>
		<a name="articles"></a>
	</h1>
	<div class="text">
	09.12.2004 - written by <a href="mailto:josef@ggzgamingzone.org">Josef Spillner</a>
	<br><br>
	GGZ has offered board games as long as the project is in existance.
	But all of these used their own methods of displaying the board, receiving
	user input and evaluating the moves.
	With GGZBoard, it has never been easier to develop board games.
	<br>
	<div class="article" style="float:right">
	<a href="ggzboard.intro.jpg"><img src="thumb.ggzboard.intro.jpg" alt="GGZBoard intro screen"></a>
	</div>
	The GGZBoard program is written in Python, and on startup it dynamically loads
	all the information it needs. For instance, if no game has yet been selected
	(that's what the command line option --game is for), a game selection screen
	appears, offering all the possible games.
	<br>
	As of now, Chess, Reversi, Hnefatafl, Go and Checkers have been implemented.
	But other board games will surely follow.
	(Beside, Chinese Checkers and Escape are already available, but were written
	before GGZBoard was invented.)
	<br>
	Using a lot of Python magic, the actual game implementation just takes a few
	lines. Basically, three parts have to be written: the game information (board
	size, pieces, initial placements), the move verification (either how to place
	pieces or how to move them), and the AI (find a move which leads to the best
	results in the long run).
	Additionally, a network module can be written, to enable GGZ support. The games
	will still be playable in single-player mode! This module only needs to list
	the network messages, and process incoming and outgoing messages.
	<br>
	<div class="article" style="float:left">
	<a href="ggzboard.chess.jpg"><img src="thumb.ggzboard.chess.jpg" alt="GGZBoard chess"></a>
	</div>
	The rest of the work is all done by GGZBoard. This includes the visual appearance
	such as window size (or fullscreen mode), background image, highlighting selected
	pieces, giving status information and so on. Also, GGZ initialization happens
	automatically when using a network module.
	<br>
	If using GGZ, statistics will be handled globally, so players can see how they
	match internationally. When running in single-player mode, a traditional local
	highscore file is used instead.
	<br>
	Due to the modular nature of the games, adding new pieces is also a breeze:
	With SDLNewStuff, all games can provide a menu for easy selection, download and
	installation of themes.
	</div>
</div>

<?php include("bottom.inc"); ?>
