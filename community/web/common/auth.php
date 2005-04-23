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
			return true;
		endif;
		return false;
	}

	function checklogin()
	{
		if ($_GET['login'] == "failed") :
			echo "<font color='#ff0000'>Error:</font>\n";
			echo "Login failed!";
		elseif ($_GET['login'] == "done"):
			$cookie = $_COOKIE[Auth::cookiename()];
			if (!$cookie) :
				echo "<font color='#ff0000'>Error:</font>\n";
				echo "Need to allow cookies!";
			endif;
		elseif ($_GET['resend'] == "done"):
			echo "<font color='#00ff00'>Notice:</font>\n";
			echo "Resent authentication credentials.";
		elseif ($_GET['resend'] == "failed"):
			echo "<font color='#ff0000'>Notice:</font>\n";
			echo "Could not authentication credentials.";
		elseif ($_GET['register'] == "failed") :
			echo "<font color='#ff0000'>Error:</font>\n";
			echo "Registration failed!";
		elseif ($_GET['register'] == "done"):
			echo "<font color='#00ff00'>Notice:</font>\n";
			echo "You have been registered.";
		elseif ($_GET['activation'] == "failed") :
			echo "<font color='#ff0000'>Error:</font>\n";
			echo "Activation failed!";
		elseif ($_GET['activation'] == "done"):
			echo "<font color='#00ff00'>Notice:</font>\n";
			echo "The new password has been activated.";
		endif;
	}

	function checkactivation($token)
	{
		global $database;

		$crypttoken = Auth::hash($token);

		$res = $database->exec("SELECT * FROM userinfo WHERE alterpass = '$crypttoken'");
		if (($res) && ($database->numrows($res) == 1)) :
			$handle = $database->result($res, 0, "handle");
			$found = 1;
			$password = dechex(rand() + time());
		endif;

		if ($found) :
			echo "Username: $handle<br>";
			echo "New password: $password<br>";
			echo "The password should be changed immediately after login!<br>";
			echo "<input type='hidden' name='input_token' value='$token'>";
			echo "<input type='hidden' name='input_activation' value='$password'>";
			echo "<input type='submit' value='Activate' class='button'>";
		else :
			echo "No such token $token found.";
		endif;
	}

	function activate($password, $token)
	{
		global $database;

		$crypttoken = Auth::hash($token);
		$cryptpass = Auth::hash($password);

		$res = $database->exec("SELECT * FROM userinfo WHERE alterpass = '$crypttoken'");
		if (($res) && ($database->numrows($res) == 1)) :
			$handle = $database->result($res, 0, "handle");
			$database->exec("UPDATE userinfo SET alterpass = '' WHERE handle = '$handle'");
			$database->exec("UPDATE users SET password = '$cryptpass' WHERE handle = '$handle'");
			return true;
		endif;

		return false;
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
			return false;
		endif;

		$cryptpass = Auth::hash($password);

		$res = $database->exec("INSERT INTO users (handle, password, permissions, name, email) " .
			"VALUES ('$username', '$cryptpass', 7, '', '$email')");
		return true;
	}

	function resend($email, $encryption)
	{
		global $database;

		$res = $database->exec("SELECT * FROM users WHERE email = '$email'");
		if (($res) && ($database->numrows($res) > 0)) :
			$text = "Resending forgotten password(s) as per request.\n";
			$text .= "\n";
			$pubkey = "";

			for ($i = 0; $i < $database->numrows($res); $i++)
			{
				$user = $database->result($res, $i, "handle");
				$password = $database->result($res, $i, "password");

				$regenerate = 1;
				$reactivate = 1;

				if ($regenerate) :
					$password = dechex(rand() + time());
					$cryptpass = Auth::hash($password);
					if ($reactivate) :
						$database->exec("UPDATE userinfo SET alterpass = '$cryptpass' WHERE handle = '$user'");
						$link = "https://www.ggzcommunity.org/login/?task=reactivate&token=$password";
						$text .= "$user: regenerated, activate on $link\n";
					else :
						$database->exec("UPDATE users SET password = '$cryptpass' WHERE handle = '$user'");
						$text .= "$user: $password (generated)\n";
					endif;
				else :
					$text .= "$user: $password\n";
				endif;

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
				$keyid = `cat $keyfile | gpg | head -1 | cut -d " " -f 3 | cut -d "/" -f 2`;
				unlink($keyfile);

				@unlink($msgfile);
				$f = fopen($msgfile, "w");
				fwrite($f, $text);
				fclose($f);
				$gpg = "gpg --armor --trust-model always --batch --encrypt";
				$text = `cat $msgfile | $gpg --recipient=$keyid`;
				unlink($msgfile);
			endif;

			$text .= "\n";
			$text .= "The GGZ Community Administrators\n";
			$text .= "mailto:info@ggzcommunity.org\n";

			mail($email, "GGZ Community: Password", $text);
			//echo "XXXXX $text";

			return true;
		else :
			return false;
		endif;
	}
}

?>
