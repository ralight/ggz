<?php

include_once("database.php");

class Auth
{
	function username()
	{
		global $database;
		static $cached_username;

		if ($cached_username) return $cached_username;

		$cookie = $_COOKIE[Auth::cookiename()];
		if (!$cookie) return;

		$res = $database->exec("SELECT * FROM auth WHERE cookie = '$cookie'");
		if (($res) && ($database->numrows($res) == 1)) :
			$username = $database->result($res, 0, "handle");

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
		global $database;

		$cryptpass = Auth::hash($password);

		$res = $database->exec("SELECT * FROM users WHERE handle = '$username' AND password = '$cryptpass'");
		if (($res) && ($database->numrows($res) == 1)) :
			$cookiestem = base64_encode(md5($cryptpass));
			$stamp = time();
			$cookie = "$cookiestem$stamp";
			$cookie = str_replace("=", "", $cookie);
			$database->exec("DELETE FROM auth WHERE handle = '$username'");
			$database->exec("INSERT INTO auth (handle, cookie) VALUES ('$username', '$cookie')");
			setcookie(Auth::cookiename(), $cookie, 0, "/");
		endif;
	}

	function logout()
	{
		global $database;

		$username = Auth::username();
		if(!$username) return;

		$database->exec("DELETE FROM auth WHERE handle = '$username'");
		setcookie(Auth::cookiename(), "", 0, "/");
	}

	function register($username, $password, $email)
	{
		global $database;

		$res = $database->exec("SELECT * FROM users WHERE handle = '$username'");
		if (($res) && ($database->numrows($res) > 0)) :
			return;
		endif;

		$cryptpass = Auth::hash($password);

		$res = $database->exec("INSERT INTO users (handle, password, permissions, name, email) " .
			"VALUES ('$username', '$cryptpass', 7, '', '$email')");
	}

	function resend($email, $encryption)
	{
		global $database;

		$res = $database->exec("SELECT * FROM users WHERE email = '$email'");
		if (($res) && ($database->numrows($res) > 0)) :
			$text = "";
			$pubkey = "";
			for ($i = 0; $i < $database->numrows($res); $i++)
			{
				$user = $database->result($res, $i, "handle");
				$password = $database->result($res, $i, "password");

				$text .= "$user: $password\n";
				if ((!$pubkey) && ($encryption)) :
					$res2 = $database->exec("SELECT * FROM userinfo WHERE handle = '$user'");
					if (($res2) && ($database->numrows($res2) == 1)) :
						$pubkey = $database->result($res2, 0, "pubkey");
					endif;
				endif;
			}
			if ($pubkey) :
				$stamp = time();
				$keyfile = "/tmp/pub.key.$stamp";
				$msgfile = "/tmp/pub.msg.$stamp";

				@unlink($keyfile);
				$f = fopen($keyfile, "w");
				fwrite($f, $pubkey);
				fclose($f);
				#$keyid = `echo $pubkey | gpg | head -1 | cut -d " " -f 3 | cut -d "/" -f 2`;
				$keyid = `cat $keyfile | gpg | head -1 | cut -d " " -f 3 | cut -d "/" -f 2`;
				unlink($keyfile);

				@unlink($msgfile);
				$f = fopen($msgfile, "w");
				fwrite($f, $text);
				fclose($f);
				#$text = `echo $text | gpg --armor --trust-model always --batch --encrypt --recipient=$keyid`;
				$text = `cat $msgfile | gpg --armor --trust-model always --batch --encrypt --recipient=$keyid`;
				unlink($msgfile);
			endif;
			mail($email, "Community: Password", $text);
		endif;
	}
}

?>
