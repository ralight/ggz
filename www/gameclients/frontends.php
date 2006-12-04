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
	case "tictactoe": $uri = "tictactoe"; break;
	case "kdots": $uri = "dots"; break;
	case "dots": $uri = "dots"; break;
	case "fyrdman": $uri = "hastings"; break;
	case "hastings": $uri = "hastings"; break;
	case "kreversi": $uri = "reversi"; break;
	case "reversi": $uri = "reversi"; break;
	case "combat": $uri = "combat"; break;
	case "koenig": $uri = "chess"; break;
	case "chess": $uri = "chess"; break;
	case "guruchess": $uri = "chess"; break;
	case "cchess": $uri = "cchess"; break;
	case "kcc": $uri = "ccheckers"; break;
	case "ccheckers": $uri = "ccheckers"; break;
	case "krosswater": $uri = "krosswater"; break;
	case "muehle": $uri = "muehle"; break;
	case "escape-sdl": $uri = "escape"; break;
	case "escape": $uri = "escape"; break;
	case "connectx": $uri = "connectx"; break;
	case "checkers": $uri = "checkers"; break;
	case "hnefatafl": $uri = "hnefatafl"; break;
	case "go": $uri = "go"; break;
	case "ludo": $uri = "ludo"; break;

	#case "ggzcards": $uri = ""; break;

	case "keepalive": $uri = "keepalive"; break;
	case "geekgame": $uri = "geekgame"; break;

	# ...
	case "pykamikaze": $uri = "kamikaze"; break;
	case "kamikaze": $uri = "kamikaze"; break;
	case "tuxman": $uri = "tuxman"; break;
	case "teg": $uri = "teg"; break;
	case "widelands": $uri = "widelands"; break;
	case "freeciv": $uri = "freeciv"; break;
	case "freelords": $uri = "freelords"; break;
	case "cgoban": $uri = "go"; break;

	# ...
	case "chinese-checkers": $uri = "ccheckers"; break;
	case "chinese-chess": $uri = "cchess"; break;
}

if ($origuri != $uri) :
	include($_SERVER['DOCUMENT_ROOT'] . "/site/header.php");
	echo "The game $origuri is a frontend for the game engine $uri.<br/>\n";
	echo "It does not yet have its own home page.<br/>\n";
	echo "Please visit the <a href='/engines/$uri/'>common $uri engine page</a>.<br/>\n";
	include($_SERVER['DOCUMENT_ROOT'] . "/site/footer.php");
else:
	include($_SERVER['DOCUMENT_ROOT'] . "/site/error.php.en");
endif;

?>
