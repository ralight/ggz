<?php

include("database.php");

class Auth
{
	function username()
	{
		global $id;
		static $cached_username;

		if ($cached_username) return $cached_username;

		$cookie = $_COOKIE[Auth::cookiename()];
		if (!$cookie) return;

		$res = pg_exec($id, "SELECT * FROM auth WHERE cookie = '$cookie'");
		if (($res) && (pg_numrows($res) == 1)) :
			$username = pg_result($res, 0, "handle");
			$cached_username = $username;
		endif;

		return $username;
	}

	function hash($password)
	{
		$cryptpass = $password; // md5($password), sha1($password)

		return $cryptpass;
	}

	function cookiename()
	{
		// no dot allowed!
		return "ggzcommunity_auth";
	}

	function login($username, $password)
	{
		global $id;

		$cryptpass = Auth::hash($password);

		$res = pg_exec($id, "SELECT * FROM users WHERE handle = '$username' AND password = '$cryptpass'");
		if (($res) && (pg_numrows($res) == 1)) :
			$cookiestem = base64_encode(md5($cryptpass));
			$stamp = time();
			$cookie = "$cookiestem$stamp";
			$cookie = str_replace("=", "", $cookie);
			pg_exec($id, "DELETE FROM auth WHERE handle = '$username'");
			pg_exec($id, "INSERT INTO auth (handle, cookie) VALUES ('$username', '$cookie')");
			setcookie(Auth::cookiename(), $cookie, 0, "/");
		endif;
	}

	function logout()
	{
		global $id;

		$username = Auth::username();
		if(!$username) return;

		pg_exec($id, "DELETE FROM auth WHERE handle = '$username'");
		setcookie(Auth::cookiename(), "", 0, "/");
	}

	function register($username, $password, $email)
	{
		global $id;

		$res = pg_exec($id, "SELECT * FROM users WHERE handle = '$username'");
		if (($res) && (pg_numrows($res) > 0)) :
			return;
		endif;

		$cryptpass = Auth::hash($password);

		$res = pg_exec($id, "INSERT INTO users (handle, password, permissions, name, email) " .
			"VALUES ('$username', '$cryptpass', 7, '', '$email')");
	}

	function resend($email)
	{
		// not implemented yet
	}
}

?>
