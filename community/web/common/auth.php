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

	function resend($email, $encryption)
	{
		global $id;

		$res = pg_exec($id, "SELECT * FROM users WHERE email = '$email'");
		if (($res) && (pg_numrows($res) > 0)) :
			$text = "";
			$pubkey = "";
			for ($i = 0; $i < pg_numrows($res); $i++)
			{
				$user = pg_result($res, $i, "handle");
				$password = pg_result($res, $i, "password");
				$text .= "$user: $password\n";
				if ((!$pubkey) && ($encryption)) :
					$res2 = pg_exec($id, "SELECT * FROM userinfo WHERE handle = '$user'");
					if (($res2) && (pg_numrows($res2) == 1)) :
						$pubkey = pg_result($res2, 0, "pubkey");
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
