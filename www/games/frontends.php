<?php

$uri = $_SERVER['REQUEST_URI'];

$uri = preg_replace("/\/games\//", "", $uri);
$uri = str_replace("/", "", $uri);

$origuri = $uri;
switch($uri)
{
	case "ktictactux": $uri = "tictactoe"; break;
	case "ttt3d": $uri = "tictactoe"; break;
	case "guruttt": $uri = "tictactoe"; break;
	case "tttxt": $uri = "tictactoe"; break;
	case "kdots": $uri = "dots"; break;
	case "fyrdman": $uri = "hastings"; break;
	case "kreversi": $uri = "reversi"; break;
	case "koenig": $uri = "chess"; break;
	case "kcc": $uri = "ccheckers"; break;
	case "escape-sdl": $uri = "escape"; break;

	case "chinese-checkers": $uri = "ccheckers"; break;
	case "chinese-chess": $uri = "cchess"; break;
}

if ($origuri != $uri) :
	include($_SERVER['DOCUMENT_ROOT'] . "/site/header.php");
	echo "The game $origuri is a frontend for the game engine $uri.<br>\n";
	echo "It does not yet have its own home page.<br>\n";
	echo "Please visit the <a href='/games/$uri/'>common $uri page</a>.<br>\n";
	include($_SERVER['DOCUMENT_ROOT'] . "/site/footer.php");
else:
	include($_SERVER['DOCUMENT_ROOT'] . "/site/error.php.en");
endif;

?>
