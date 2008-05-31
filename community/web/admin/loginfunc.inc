<?php

function default_realm()
{
	//$realm = 'GGZ Community';
	return 'GGZ Community';
}

// function to parse the http auth header
function http_digest_parse($txt)
{
	// protect against missing data
	$needed_parts = array(
		'nonce' => 1,
		'nc' => 1,
		'cnonce' => 1,
		'qop' => 1,
		'username' => 1,
		'uri' => 1,
		'response' => 1
	);
	$data = array();

	// FIXME: original PHP (from features.http-auth.php) had [^\2] instead of .+? which doesn't work
	preg_match_all('@(\w+)=(?:([\'"])(.+?)\2|([^\s,]+))@', $txt, $matches, PREG_SET_ORDER);

	foreach ($matches as $m)
	{
		$data[$m[1]] = $m[3] ? $m[3] : $m[4];
		unset($needed_parts[$m[1]]);
	}

	return $needed_parts ? false : $data;
}

function request_authorization($realm)
{
	header('HTTP/1.1 401 Unauthorized');
	header('WWW-Authenticate: Digest realm="' . $realm .
		'",qop="auth",nonce="' . uniqid() . '",opaque="' . md5($realm) . '"');
}

function checkauthentication($realm)
{
	global $database;

	$users = array();
	$errorstr = "";
	$authrequest = false;

	// FIXME: manual query on rooms_admin
	if ($database) :
		$res = $database->exec("SELECT handle, password FROM users WHERE (permissions & 8) = 8", array());
	else :
		$res = false;
	endif;

	if ($res) :
		if ($database->numrows($res) > 0) :
			for ($i = 0; $i < $database->numrows($res); $i++)
			{
				$handle = $database->result($res, $i, "handle");
				$password = $database->result($res, $i, "password");

				$users[$handle] = $password;
			}
		else :
			$errorstr = "No administrators found in the database";
		endif;
	else :
		$errorstr = "Failed connection to the database";
	endif;

	if (!$errorstr) :
		if (empty($_SERVER['PHP_AUTH_DIGEST'])) :
			$authrequest = true;
			$errorstr = "Good luck next time";
		endif;
	endif;

	if (!$errorstr) :
		// analyze the PHP_AUTH_DIGEST variable
		if ((!($data = http_digest_parse($_SERVER['PHP_AUTH_DIGEST'])))
		|| (!isset($users[$data['username']]))) :
			// FIXME: these headers seem to be necessary for konqueror/firefox - RFC compliance?
			$authrequest = true;
			$errorstr = "Wrong Credentials!";
		endif;
	endif;

	if (!$errorstr) :
		// generate the valid response
		$A1 = md5($data['username'] . ':' . $realm . ':' . $users[$data['username']]);
		$A2 = md5($_SERVER['REQUEST_METHOD'] . ':' . $data['uri']);
		$AX = $data['nonce'] . ':' . $data['nc'] . ':' . $data['cnonce'] . ':' . $data['qop'];
		$valid_response = md5($A1 . ':' . $AX . ':' . $A2);

		if ($data['response'] != $valid_response) :
			// FIXME: these headers seem to be necessary for konqueror/firefox - RFC compliance?
			$authrequest = true;
			$errorstr = "Wrong Credentials!";
		endif;
	endif;

	return array(
		"errorstr" => $errorstr,
		"authrequest" => $authrequest,
		"username" => $data['username']
	);
}


?>
