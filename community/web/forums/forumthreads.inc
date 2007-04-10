<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

$forumid = $lookup;
$res = $database->exec("SELECT * FROM forums WHERE id = '%^'", array($forumid));
if ($database->numrows($res) == 1):
	$forumname = $database->result($res, $i, "name");
	$forumclub = $database->result($res, $i, "club");
endif;

if ($forumclub) :
	$res = $database->exec("SELECT * FROM teammembers WHERE teamname = '%^' AND handle = '%^'",
		array($forumclub, Auth::username()));
	if ($database->numrows($res) != 1) :
		// FIXME: how to exit gracefully?
		echo "You don't have permission to view this forum.";
		echo "</div>";
		include("bottom.inc");
		exit;
	endif;
endif;
?>
<?php
if ($_POST['forum_text']) :
	if (Auth::username()) :
		$database->exec("INSERT INTO forumthreads (forum, thread, title) " .
			"VALUES ('%^', '%^', '%^')",
			array($forumid, 0, $_POST['forum_title']));
		$res = $database->exec("SELECT id FROM forumthreads WHERE forum = '%^' AND title = '%^'",
			array($forumid, $_POST['forum_title']));
		if ($database->numrows($res) == 1) :
			$threadid = $database->result($res, 0, "id");
			$stamp = time();
			$database->exec("INSERT INTO forummessages (sender, date, title, message) " .
				"VALUES ('%^', '%^', '%^', '%^')",
				array(Auth::username(), $stamp, $_POST['forum_title'], $_POST['forum_text']));
			$res = $database->exec("SELECT id FROM forummessages WHERE date = '%^' AND sender = '%^'",
				array($stamp, Auth::username()));
			if ($database->numrows($res) == 1) :
				$messageid = $database->result($res, 0, "id");
				$database->exec("INSERT INTO forumthreadmessages (thread, message) " .
					"VALUES ('%^', '%^')",
					array($threadid, $messageid));
			endif;
		endif;
	endif;
endif;
?>
	<h2>
		<span class="itemleader">:: </span>
		List of threads in forum <?php echo $forumname; ?>
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h2>
	<div class="text">
<?php
$res = $database->exec("SELECT * FROM forumthreads WHERE forum = '%^'", array($forumid));

for ($i = 0; $i < $database->numrows($res); $i++)
{
	$threadid = $database->result($res, $i, "id");
	$title = $database->result($res, $i, "title");

	$res2 = $database->exec("SELECT COUNT(*) FROM forumthreadmessages WHERE thread = '%^'", array($threadid));
	$count = "???";
	if ($database->numrows($res2) == 1) :
		$count = $database->result($res2, 0, "count");
	endif;

	echo "<a href='ggzforum.php?lookup=$forumid&thread=$threadid'>$title</a> - $count posts<br>\n";
}
?>
	</div>
	<div class="text">
<?php
if (Auth::username()) :
?>
	New thread with title:
	<br>
	<form method='post' action='ggzforum.php?lookup=<?php echo $forumid; ?>'>
	<input name='forum_title' size=80>
	<br>
	Text:
	<br>
	<textarea name='forum_text' rows='12' cols='80'></textarea>
	<br>
	<input type='submit' value='Submit'>
	</form>
<?php
else :
?>
	You need to be logged in to open a new thread.
<?php
endif;
?>
	</div>
