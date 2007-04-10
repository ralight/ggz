<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

$forumid = $lookup;
$threadid = $thread;
$res = $database->exec("SELECT * FROM forumthreads WHERE forum = '%^' AND id = '%^'",
	array($forumid, $threadid));
if ($database->numrows($res) == 1):
	$title = $database->result($res, $i, "title");
endif;

// FIXME: the checks below are somewhat replicated from forumthreads.inc
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
		$stamp = time();
		$database->exec("INSERT INTO forummessages (sender, date, title, message) " .
			"VALUES ('%^', '%^', '%^', '%^')",
			array(Auth::username(), $stamp, "", $_POST['forum_text']));
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
?>
	<h2>
		<span class="itemleader">:: </span>
		Messages in thread <?php echo $title; ?>
		<span class="itemleader"> :: </span>
		<a name="database"></a>
	</h2>
	<div class="text">
<?php
$res = $database->exec("SELECT * FROM forumthreadmessages WHERE thread = '%^'", array($threadid));

for ($i = 0; $i < $database->numrows($res); $i++)
{
	$messageid = $database->result($res, $i, "message");

	$res2 = $database->exec("SELECT * FROM forummessages WHERE id = '%^'", array($messageid));
	$sender = $database->result($res2, 0, "sender");
	$title = $database->result($res2, 0, "title");
	$text = $database->result($res2, 0, "message");

?>
	</div>
	<div class="text">
<?
	echo "<b>$title</b> (posted by $sender)<br>\n";
	echo "$text<br>\n";

	echo "<br>\n";
}
?>
	</div>

	<div class="text">
<?php
if (Auth::username()) :
?>
	Reply:
	<br>
	<form method='post' action='ggzforum.php?lookup=<?php echo $forumid; ?>&thread=<?php echo $threadid; ?>'>
	<textarea name='forum_text' rows='12' cols='80'></textarea>
	<br>
	<input type='submit' value='Submit'>
	</form>
<?php
else :
?>
	You need to be logged in to post a reply.
<?php
endif;
?>
	</div>

