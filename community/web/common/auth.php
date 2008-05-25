<?php

include($_SERVER['DOCUMENT_ROOT']."/common/include_cfg.php");

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

		$res = $database->exec("SELECT * FROM auth WHERE cookie = '%^'", array($cookie));
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

	function language()
	{
		return $_COOKIE["ggzcommunity_lang"];
	}

	function setlanguage($lang)
	{
		setcookie("ggzcommunity_lang", $lang, 0, "/");
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

		$res = $database->exec("SELECT * FROM users WHERE handle = '%^' AND password = '%^'", array($username, $cryptpass));
		if (($res) && ($database->numrows($res) == 1)) :
			if (Config::getvalue("registration") == "confirm") :
				$confirmed = $database->result($res, 0, "confirmed");
				if ($confirmed == 'f') :
					return false;
				endif;
			endif;
			$cookiestem = base64_encode(md5($cryptpass));
			$stamp = time();
			$cookie = "$cookiestem$stamp";
			$cookie = str_replace("=", "", $cookie);
			$database->exec("DELETE FROM auth WHERE handle = '%^'", array($username));
			$database->exec("INSERT INTO auth (handle, cookie) VALUES ('%^', '%^')", array($username, $cookie));
			setcookie(Auth::cookiename(), $cookie, 0, "/");
			return true;
		endif;
		return false;
	}

	function checklogin()
	{
		$error = __("Error:");
		$notice = __("Notice:");

		if ($_GET['login'] == "failed") :
			echo "<font color='#ff0000'>$error</font>\n";
			echo __("Login failed!");
		elseif ($_GET['login'] == "done"):
			$cookie = $_COOKIE[Auth::cookiename()];
			if (!$cookie) :
				echo "<font color='#ff0000'>$error</font>\n";
				echo __("Need to allow cookies!");
			endif;
		elseif ($_GET['resend'] == "done"):
			echo "<font color='#00ff00'>$notice</font>\n";
			echo __("Resent authentication credentials.");
		elseif ($_GET['resend'] == "failed"):
			echo "<font color='#ff0000'>$notice</font>\n";
			echo __("Could not authentication credentials.");
		elseif ($_GET['register'] == "failed") :
			echo "<font color='#ff0000'>$error</font>\n";
			echo __("Registration failed!");
		elseif ($_GET['register'] == "done"):
			echo "<font color='#00ff00'>$notice</font>\n";
			echo __("You have been registered.");
			if (Config::getvalue("registration") == "confirm") :
				echo __("Check your email to activate your account.");
			elseif (Config::getvalue("registration") == "confirmlater") :
				echo __("Don't forget to activate your account soon.");
			endif;
		elseif ($_GET['activation'] == "failed") :
			echo "<font color='#ff0000'>$error</font>\n";
			echo __("Activation failed!");
		elseif ($_GET['activation'] == "done"):
			echo "<font color='#00ff00'>$notice</font>\n";
			echo __("The new password has been activated.");
		endif;
	}

	function checkactivation($token)
	{
		global $database;

		$crypttoken = Auth::hash($token);

		$res = $database->exec("SELECT * FROM userinfo WHERE alterpass = '%^'", array($crypttoken));
		if (($res) && ($database->numrows($res) == 1)) :
			$handle = $database->result($res, 0, "handle");
			$found = 1;
			$password = dechex(rand() + time());
		endif;

		if ($found) :
			echo __("Username:") . " $handle<br>";
			echo __("New password:") . " $password<br>";
			echo __("The password should be changed immediately after login!") . "<br>";
			echo "<input type='hidden' name='input_token' value='$token'>";
			echo "<input type='hidden' name='input_activation' value='$password'>";
			echo "<input type='submit' value='" . __("Activate") . "' class='button'>";
		else :
			echo __("No such token $token found.");
		endif;
	}

	function checkconfirmation($token)
	{
		global $database;

		$crypttoken = Auth::hash($token);

		$res = $database->exec("SELECT * FROM userinfo WHERE alterpass = '%^'", array($crypttoken));
		if (($res) && ($database->numrows($res) == 1)) :
			$handle = $database->result($res, 0, "handle");
			$database->exec("UPDATE userinfo SET alterpass = '' WHERE handle = '%^'", array($handle));
			$database->exec("UPDATE users SET confirmed = 't' WHERE handle = '%^'", array($handle));
			$found = 1;
		endif;

		if ($found) :
			echo __("The account is now active.") . "<br>";
		else :
			echo __("No such token $token found.");
		endif;
	}

	function activate($password, $token)
	{
		global $database;

		$crypttoken = Auth::hash($token);
		$cryptpass = Auth::hash($password);

		$res = $database->exec("SELECT * FROM userinfo WHERE alterpass = '%^'", array($crypttoken));
		if (($res) && ($database->numrows($res) == 1)) :
			$handle = $database->result($res, 0, "handle");
			$database->exec("UPDATE userinfo SET alterpass = '' WHERE handle = '%^'", array($handle));
			$database->exec("UPDATE users SET password = '%^' WHERE handle = '%^'", array($cryptpass, $handle));
			return true;
		endif;

		return false;
	}

	function logout()
	{
		global $database;

		$username = Auth::username();
		if(!$username) return;

		$database->exec("DELETE FROM auth WHERE handle = '%^'", array($username));
		setcookie(Auth::cookiename(), "", 0, "/");
	}

	function register($username, $password, $email, $realname)
	{
		global $database;

		if (Config::getvalue("registration") == "closed") :
			return false;
		endif;

		$res = $database->exec("SELECT * FROM users WHERE handle = '%^'", array($username));
		if (($res) && ($database->numrows($res) > 0)) :
			return false;
		endif;

		$cryptpass = Auth::hash($password);

		$confirmed = 't';
		if (Config::getvalue("registration") != "open") :
			$confirmed = 'f';
		endif;

		$res = $database->exec("INSERT INTO users (handle, password, permissions, name, email, confirmed, firstlogin) " .
			"VALUES ('%^', '%^', %^, '%^', '%^', '%^', '%^')",
			array($username, $cryptpass, 7, $realname, $email, $confirmed, time()));

		if (Config::getvalue("registration") != "open") :
			Auth::requestconfirmation($email);
		endif;

		return true;
	}

	function requestconfirmation($email)
	{
		global $database;

		$res = $database->exec("SELECT * FROM users WHERE email = '%^'", array($email));
		if (($res) && ($database->numrows($res) > 0)) :
			$user = $database->result($res, 0, "handle");
			$password = dechex(rand() + time());
			$cryptpass = Auth::hash($password);
			$database->exec("INSERT INTO userinfo (handle, alterpass) VALUES ('%^', '%^')",
				array($user, $cryptpass));
			$link = Config::getvalue("url") .  "/login/?task=confirm&token=$password";

			$text = __("Account confirmation mail.") . "\n";
			$text .= "\n";
			$text .= __("To activate your account, visit $link\n");

			$name = Config::getvalue("name");
			$text .= "\n";
			$text .= __("The $name Administrators") . "\n";
			$text .= "mailto:" . Config::getvalue("mail") . "\n";

			mail($email, "$name: " . ("Account confirmation"), $text);

			return true;
		else :
			return false;
		endif;
	}

	function resend($email, $encryption)
	{
		global $database;

		$res = $database->exec("SELECT * FROM users WHERE email = '%^'", array($email));
		if (($res) && ($database->numrows($res) > 0)) :
			$text = __("Resending forgotten password(s) as per request.") . "\n";
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
						$database->exec("UPDATE userinfo SET alterpass = '%^' WHERE handle = '%^'", array($cryptpass, $user));
						$link = Config::getvalue("url") .  "/login/?task=reactivate&token=$password";
						$text .= __("$user: regenerated, activate on $link\n");
					else :
						$database->exec("UPDATE users SET password = '%^' WHERE handle = '%^'", array($cryptpass, $user));
						$text .= __("$user: $password (generated)\n");
					endif;
				else :
					$text .= "$user: $password\n";
				endif;

				if ((!$pubkey) && ($encryption)) :
					$res2 = $database->exec("SELECT * FROM userinfo WHERE handle = '%^'", array($user));
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

			$name = Config::getvalue("name");
			$text .= "\n";
			$text .= __("The $name Administrators") . "\n";
			$text .= "mailto:" . Config::getvalue("mail") . "\n";

			mail($email, "$name: " . ("Password"), $text);

			return true;
		else :
			return false;
		endif;
	}
}

?>
