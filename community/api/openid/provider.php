<?php
/* GGZ Community OpenID provider
 *
 * Based on:
 * phpMyID - A standalone, single user, OpenID Identity Provider
 * (C) 2006-2007 CJ Niemira <siege (at) siege (dot) org>
 * http://siege.org/projects/phpMyID
 * Version: 0.5
 *
 * This code is licensed under the GNU General Public License
 * http://www.gnu.org/licenses/gpl.html
 *
 *
 * *************************************************************************** *
 * CONFIGURATION
 * *************************************************************************** *
 * You must change these values:
 *	auth_username = login name
 *	auth_password = md5(username:realm:password)
 *
 * Default username = 'test', password = 'test', realm = 'phpMyID'
 */

$profile = array(
	'auth_username'	=> 	'test',
	'auth_password' =>	'37fa04faebe5249023ed1f6cc867329b'
);

/*
 * Optional - Simple Registration Extension:
 *
 *   If you would like to add any of the following optional registration
 *   parameters to your login profile, simply uncomment the line, and enter the
 *   correct values.
 *
 *   Details on the exact allowed values for these paramters can be found at:
 *   http://openid.net/specs/openid-simple-registration-extension-1_0.html
 */

#$sreg = array (
#	'nickname'		=> 'Joe',
#	'email'			=> 'joe@example.com',
#	'fullname'		=> 'Joe Example',
#	'dob'			=> '1970-10-31',
#	'gender'		=> 'M',
#	'postcode'		=> '22000',
#	'country'		=> 'US',
#	'language'		=> 'en',
#	'timezone'		=> 'America/New_York'
#);



/******************************************************************************/

/*
 * Internal configuration
 * DO NOT ALTER ANYTHING BELOW THIS POINT UNLESS YOU KNOW WHAT YOU ARE DOING!
 */
define('PHPMYID_STARTED', true);

$known = array(
	'assoc_types'	=> array('HMAC-SHA1'),

	'openid_modes'	=> array('associate',
				 'authorize',
				 'cancel',
				 'checkid_immediate',
				 'checkid_setup',
				 'check_authentication',
				 'error',
				 'id_res',
				 'login',
			 	 'logout'),

	'session_types'	=> array('',
				 'DH-SHA1'),

	'bigmath_types' => array('DH-SHA1'),
);

$g = 2;

$p = '15517289818147369747123225776371553991572480196691540447970779531405762' .
'9378541917580651227423698188993727816152646631438561595825688188889951272158' .
'8426754199503412587065565498035801048705376814767265132557470407658574792912' .
'9157233451064324509471500722962109419434978392598476037559498584825335930558' .
'5439638443';



/******************************************************************************/

/*
 * Runmode functions
 */

function associate_mode () {
	global $g, $known, $p, $profile;

	// Validate the request
	if (! isset($_POST['openid_mode']) || $_POST['openid_mode'] != 'associate')
		error_400();

	// Get the options, use defaults as necessary
	$assoc_type = (@strlen($_POST['openid_assoc_type'])
		    && in_array($_POST['openid_assoc_type'], $known['assoc_types']))
			? $_POST['openid_assoc_type']
			: 'HMAC-SHA1';

	$session_type = (@strlen($_POST['openid_session_type'])
		      && in_array($_POST['openid_session_type'], $known['session_types']))
			? $_POST['openid_session_type']
			: '';

	$dh_modulus = (@strlen($_POST['openid_dh_modulus']))
		? $_POST['openid_dh_modulus']
		: ($session_type == 'DH-SHA1'
			? $p
			: null);

	$dh_gen = (@strlen($_POST['openid_dh_gen']))
		? $_POST['openid_dh_gen']
		: ($session_type == 'DH-SHA1'
			? $g
			: null);

	$dh_consumer_public = (@strlen($_POST['openid_dh_consumer_public']))
		? $_POST['openid_dh_consumer_public']
		: ($session_type == 'DH-SHA1'
			? error_post('dh_consumer_public was not specified')
			: null);

	// Create the associate id and shared secret now
	$lifetime = time() + $profile['lifetime'];

	// Create standard keys
	$keys = array(
		'assoc_type' => $assoc_type,
		'expires_in' => $profile['lifetime']
	);

	// If I can't handle bigmath, default to plaintext sessions
	if (in_array($session_type, $known['bigmath_types']) && ! extension_loaded('bcmath'))
		$session_type = null;

	// Add response keys based on the session type
	switch ($session_type) {
		case 'DH-SHA1':
			list ($assoc_handle, $shared_secret) = new_assoc($lifetime);

			// Compute the Diffie-Hellman stuff
			$private_key = random($dh_modulus);
			$public_key = bcpowmod($dh_gen, $private_key, $dh_modulus);
			$remote_key = long(base64_decode($dh_consumer_public));
			$ss = bcpowmod($remote_key, $private_key, $dh_modulus);

			$keys['assoc_handle'] = $assoc_handle;
			$keys['session_type'] = $session_type;
			$keys['dh_server_public'] = base64_encode(bin($public_key));
			$keys['enc_mac_key'] = base64_encode(x_or(sha1_20(bin($ss)), $shared_secret));

			break;

		default:
			list ($assoc_handle, $shared_secret) = new_assoc();

			$keys['assoc_handle'] = $assoc_handle;
			$keys['mac_key'] = base64_encode($shared_secret);
	}

	// Return the keys
	wrap_kv($keys);
}


function authorize_mode () {
	global $profile;

	user_session();

	// the user needs refresh urls in their session to access this mode
	if (! isset($_SESSION['post_auth_url']) || ! isset($_SESSION['cancel_auth_url']))
		error_500('You may not access this mode directly.');

	// try to get the digest headers - what a PITA!
	if (function_exists('apache_request_headers') && ini_get('safe_mode') == false) {
		$arh = apache_request_headers();
		$hdr = $arh['Authorization'];

	} elseif (isset($_SERVER['PHP_AUTH_DIGEST'])) {
		$hdr = $_SERVER['PHP_AUTH_DIGEST'];

	} elseif (isset($_SERVER['HTTP_AUTHORIZATION'])) {
		$hdr = $_SERVER['HTTP_AUTHORIZATION'];

	} elseif (isset($_ENV['PHP_AUTH_DIGEST'])) {
		$hdr = $_ENV['PHP_AUTH_DIGEST'];

	} elseif (isset($_GET['auth'])) {
		$hdr = stripslashes(urldecode($_GET['auth']));

	} else {
		$hdr = null;
	}

	debug('Authorization header: ' . $hdr);
	$digest = substr($hdr,0,7) == 'Digest '
		?  substr($hdr, strpos($hdr, ' ') + 1)
		: $hdr;

	$stale = false;

	// is the user trying to log in?
	if (! is_null($digest) && $profile['authorized'] === false) {
		debug('Digest headers: ' . $digest);
		$hdr = array();

		// decode the Digest authorization headers
		preg_match_all('/(\w+)=(?:"([^"]+)"|([^\s,]+))/', $digest, $mtx, PREG_SET_ORDER);

		foreach ($mtx as $m)
			$hdr[$m[1]] = $m[2] ? $m[2] : $m[3];
		debug($hdr, 'Parsed digest headers:');

		if (isset($_SESSION['uniqid']) && $hdr['nonce'] != $_SESSION['uniqid']) {
			$stale = true;
			unset($_SESSION['uniqid']);
		}

		if (! isset($_SESSION['failures']))
			$_SESSION['failures'] = 0;

		if ($profile['auth_username'] == $hdr['username'] && ! $stale) {

			// the entity body should always be null in this case
			$entity_body = '';
			$a1 = strtolower($profile['auth_password']);
			$a2 = $hdr['qop'] == 'auth-int'
				? md5(implode(':', array($_SERVER['REQUEST_METHOD'], $hdr['uri'], md5($entity_body))))
				: md5(implode(':', array($_SERVER['REQUEST_METHOD'], $hdr['uri'])));
			$ok = md5(implode(':', array($a1, $hdr['nonce'], $hdr['nc'], $hdr['cnonce'], $hdr['qop'], $a2)));

			// successful login!
			if ($hdr['response'] == $ok) {
				debug('Authentication successful');
				debug('User session is: ' . session_id());
				$_SESSION['auth_username'] = $hdr['username'];
				$_SESSION['auth_url'] = $profile['idp_url'];
				$profile['authorized'] = true;

				// return to the refresh url if they get in
				wrap_refresh($_SESSION['post_auth_url']);

			// too many failures
			} elseif (strcmp($hdr['nc'], 4) > 0 || $_SESSION['failures'] > 4) {
				debug('Too many password failures');
				error_get($_SESSION['cancel_auth_url'], 'Too many password failures. Double check your authorization realm. You must restart your browser to try again.');

			// failed login
			} else {
				$_SESSION['failures']++;
				debug('Login failed: ' . $hdr['response'] . ' != ' . $ok);
				debug('Fail count: ' . $_SESSION['failures']);
			}
		}

	} elseif (is_null($digest) && $profile['authorized'] === false && isset($_SESSION['uniqid'])) {
		error_500('Missing expected authorization header.');
	}

	// if we get this far the user is not authorized, so send the headers
	$uid = uniqid(mt_rand(1,9));
	$_SESSION['uniqid'] = $uid;

	debug('Prompting user to log in. Stale? ' . $stale);
	header('HTTP/1.0 401 Unauthorized');
	header(sprintf('WWW-Authenticate: Digest qop="auth-int, auth", realm="%s", domain="%s", nonce="%s", opaque="%s", stale="%s", algorithm="MD5"', $profile['auth_realm'], $profile['auth_domain'], $uid, md5($profile['auth_realm']), $stale ? 'true' : 'false'));
	$q = strpos($_SESSION['cancel_auth_url'], '?') ? '&' : '?';
	wrap_refresh($_SESSION['cancel_auth_url'] . $q . 'openid.mode=cancel');
}


function cancel_mode () {
	wrap_html('Request cancelled.');
}


function check_authentication_mode () {
	// Validate the request
	if (! isset($_POST['openid_mode']) || $_POST['openid_mode'] != 'check_authentication')
		error_400();

	$assoc_handle = @strlen($_POST['openid_assoc_handle'])
		? $_POST['openid_assoc_handle']
		: error_post('Missing assoc_handle');

	$sig = @strlen($_POST['openid_sig'])
		? $_POST['openid_sig']
		: error_post('Missing sig');

	$signed = @strlen($_POST['openid_signed'])
		? $_POST['openid_signed']
		: error_post('Missing signed');

	// Prepare the return keys
	$keys = array(
		'openid.mode' => 'id_res'
	);

	// Invalidate the assoc handle if we need to
	if (@strlen($_POST['openid_invalidate_handle'])) {
		destroy_assoc_handle($_POST['openid_invalidate_handle']);

		$keys['invalidate_handle'] = $_POST['openid_invalidate_handle'];
	}

	// Validate the sig by recreating the kv pair and signing
	$_POST['openid_mode'] = 'id_res';
	$tokens = '';
	foreach (explode(',', $signed) as $param) {
		$post = preg_replace('/\./', '_', $param);
		$tokens .= sprintf("%s:%s\n", $param, $_POST['openid_' . $post]);
	}

	// Add the sreg stuff, if we've got it
	foreach (explode(',', $sreg_required) as $key) {
			if (! isset($sreg[$key]))
				continue;
			$skey = 'sreg.' . $key;

			$tokens .= sprintf("%s:%s\n", $skey, $sreg[$key]);
			$keys[$skey] = $sreg[$key];
			$fields[] = $skey;
	}

	list ($shared_secret, $expires) = secret($assoc_handle);

	// A 'smart mode' id will have an expiration time set, don't allow it
	if ($shared_secret == false || is_numeric($expires)) {
		$keys['is_valid'] = 'false';

	} else {
		$ok = base64_encode(hmac($shared_secret, $tokens));
		$keys['is_valid'] = ($sig == $ok) ? 'true' : 'false';
	}

	// Return the keys
	wrap_kv($keys);
}


function checkid ( $wait ) {
	debug("checkid : $wait");
	global $known, $profile, $sreg;

	user_session();

	// Get the options, use defaults as necessary
	$return_to = @strlen($_GET['openid_return_to'])
		? $_GET['openid_return_to']
		: error_400('Missing return_to');

	$identity = @strlen($_GET['openid_identity'])
			? $_GET['openid_identity']
			: error_get($return_to, 'Missing identity');

	$assoc_handle = @strlen($_GET['openid_assoc_handle'])
			? $_GET['openid_assoc_handle']
			: null;

	$trust_root = @strlen($_GET['openid_trust_root'])
			? $_GET['openid_trust_root']
			: $return_to;

	$sreg_required = @strlen($_GET['openid_sreg_required'])
			? $_GET['openid_sreg_required']
			: '';

	$sreg_optional = @strlen($_GET['openid_sreg_optional'])
			? $_GET['openid_sreg_optional']
			: '';

	// required and optional make no difference to us
	$sreg_required .= ',' . $sreg_optional;

	// make sure i am this identifier
	if ($identity != $profile['idp_url'])
		error_get($return_to, "Invalid identity: '$identity'");

	// begin setting up return keys
	$keys = array(
		'mode' => 'id_res'
	);

	// if the user is not logged in, transfer to the authorization mode
	if ($profile['authorized'] === false || $identity != $_SESSION['auth_url']) {
		// users can only be logged in to one url at a time
		$_SESSION['auth_username'] = null;
		$_SESSION['auth_url'] = null;

		if ($wait) {
			unset($_SESSION['uniqid']);

			$_SESSION['cancel_auth_url'] = $return_to;
			$_SESSION['post_auth_url'] = $profile['req_url'];

			debug('Transferring to authorization mode.');
			debug('Cancel URL: ' . $_SESSION['cancel_auth_url']);
			debug('Post URL: ' . $_SESSION['post_auth_url']);

			$q = strpos($profile['idp_url'], '?') ? '&' : '?';
			wrap_refresh($profile['idp_url'] . $q . 'openid.mode=authorize');
		} else {
			$keys['user_setup_url'] = $profile['idp_url'];
		}

	// the user is logged in
	} else {
		// remove the refresh URLs if set
		unset($_SESSION['cancel_auth_url']);
		unset($_SESSION['post_auth_url']);

		// check the assoc handle
		list($shared_secret, $expires) = secret($assoc_handle);
		if ($shared_secret == false || (is_numeric($expires) && $expires < time())) {
			if ($assoc_handle != null)
				$keys['invalidate_handle'] = $assoc_handle;
			list ($assoc_handle, $shared_secret) = new_assoc();
		}

		$keys['identity'] = $profile['idp_url'];
		$keys['assoc_handle'] = $assoc_handle;
		$keys['return_to'] = $return_to;

		$fields = array_keys($keys);
		$tokens = '';
		foreach ($fields as $key)
			$tokens .= sprintf("%s:%s\n", $key, $keys[$key]);

		// add sreg keys
		foreach (explode(',', $sreg_required) as $key) {
			if (! isset($sreg[$key]))
				continue;
			$skey = 'sreg.' . $key;

			$tokens .= sprintf("%s:%s\n", $skey, $sreg[$key]);
			$keys[$skey] = $sreg[$key];
			$fields[] = $skey;
		}

		$keys['signed'] = implode(',', $fields);
		$keys['sig'] = base64_encode(hmac($shared_secret, $tokens));
	}

	wrap_location($return_to, $keys);
}


function checkid_immediate_mode () {
	if (! isset($_GET['openid_mode']) || $_GET['openid_mode'] != 'checkid_immediate')
		error_500();

	checkid(false);
}


function checkid_setup_mode () {
	if (! isset($_GET['openid_mode']) || $_GET['openid_mode'] != 'checkid_setup')
		error_500();

	checkid(true);
}


function error_mode () {
	isset($_REQUEST['openid_error']) 
		? wrap_html($_REQUEST['openid_error'])
		: error_500();
}


function id_res_mode () {
	global $profile;

	user_session();

	if ($profile['authorized'])
		wrap_html('You are logged in as ' . $_SESSION['auth_username']);

	wrap_html('You are not logged in');
}


function login_mode () {
	global $profile;

	user_session();

	if ($profile['authorized'])
		id_res_mode();

	$keys = array(
		'mode' => 'checkid_setup',
		'identity' => $profile['idp_url'],
		'return_to' => $profile['idp_url']
	);

	wrap_location($profile['idp_url'], $keys);
}


function logout_mode () {
	global $profile;

	user_session();

	if (! $profile['authorized'])
		wrap_html('You were not logged in');

	$_SESSION = array();
	session_destroy();
	debug('User session destroyed.');

	header('HTTP/1.0 401 Unauthorized');
	wrap_refresh($profile['idp_url']);
}


function no_mode () {
	global $profile;

	wrap_html('This is an OpenID server endpoint. For more information, see http://openid.net/<br/>Server: ' . $profile['idp_url'] . '<br/>Realm: ' . $profile['php_realm']);
}



/*
 * Support functions
 */
function append_openid ($array) {
	$keys = array_keys($array);
	$vals = array_values($array);

	$r = array();
	for ($i=0; $i<sizeof($keys); $i++)
		$r['openid.' . $keys[$i]] = $vals[$i];
	return $r;
}

// Borrowed from http://php.net/manual/en/function.bcpowmod.php#57241
if (! function_exists('bcpowmod')) {
function bcpowmod ($value, $exponent, $mod) {
	$r = 1;
	while (true) {
		if (bcmod($exponent, 2) == "1")
			break;
		if (($exponent = bcdiv($exponent, 2)) == '0')
			break;
		$value = bcmod(bcmul($value, $value), $mod);
	}
	return $r;
}}


// Borrowed from PHP-OpenID; http://openidenabled.com
function bin ($n) {
	$bytes = array();
	while (bccomp($n, 0) > 0) {
		array_unshift($bytes, bcmod($n, 256));
		$n = bcdiv($n, pow(2,8));
	}

	if ($bytes && ($bytes[0] > 127))
		array_unshift($bytes, 0);

	$b = '';
	foreach ($bytes as $byte)
		$b .= pack('C', $byte);

	return $b;
}


function debug ($x, $m = null) {
	global $profile;

	if (! isset($profile['debug']) || $profile['debug'] === false)
		return true;

	if (! is_writable(dirname($profile['logfile'])) &! is_writable($profile['logfile']))
		error_500('Cannot write to debug log: ' . $profile['logfile']);

	if (is_array($x)) {
		ob_start();
		print_r($x);
		$x = $m . ($m != null ? "\n" : '') . ob_get_clean();

	} else {
		$x .= "\n";
	}

	error_log($x . "\n", 3, $profile['logfile']);
}


function destroy_assoc_handle ( $id ) {
	debug("Destroying session: $id");

	$sid = session_id();
	session_write_close();

	session_id($id);
	session_start();
	session_destroy();

	session_id($sid);
	session_start();
}


function error_400 ( $message = 'Bad Request' ) {
	header("HTTP/1.1 400 Bad Request");
	wrap_html($message);
}


function error_500 ( $message = 'Internal Server Error' ) {
	header("HTTP/1.1 500 Internal Server Error");
	wrap_html($message);
}


function error_get ( $url, $message = 'Bad Request') {
	wrap_location($url, array('mode' => 'error', 'error' => $message));
}


function error_post ( $message = 'Bad Request' ) {
	header("HTTP/1.1 400 Bad Request");
	echo ('error:' . $message);
	exit(0);
}


// Borrowed from - http://php.net/manual/en/function.sha1.php#39492
function hmac($key, $data, $hash = 'sha1_20') {
	$blocksize=64;

	if (strlen($key) > $blocksize)
		$key = $hash($key);

	$key = str_pad($key, $blocksize,chr(0x00));
	$ipad = str_repeat(chr(0x36),$blocksize);
	$opad = str_repeat(chr(0x5c),$blocksize);

	$h1 = $hash(($key ^ $ipad) . $data);
	$hmac = $hash(($key ^ $opad) . $h1);
	return $hmac;
}


if (! function_exists('http_build_query')) {
function http_build_query ($array) {
	$r = array();
	foreach ($array as $key => $val)
		$r[] = sprintf('%s=%s', $key, urlencode($val));
	return implode('&', $r);
}}


// Borrowed from PHP-OpenID; http://openidenabled.com
function long($b) {
	$bytes = array_merge(unpack('C*', $b));
	$n = 0;
	foreach ($bytes as $byte) {
		$n = bcmul($n, bcpow(2,8));
		$n = bcadd($n, $byte);
	}
	return $n;
}


function new_assoc ( $expiration = null ) {
	if (is_array($_SESSION)) {
		$sid = session_id();
		$dat = session_encode();
		session_write_close();
	}

	session_start();
	session_regenerate_id('false');

	$id = session_id();
	$shared_secret = new_secret();
	debug('Started new assoc session: ' . $id);

	$_SESSION = array();
	$_SESSION['expiration'] = $expiration;
	$_SESSION['shared_secret'] = base64_encode($shared_secret);

	session_write_close();

	if (isset($sid)) {
		session_id($sid);
		session_start();
		$_SESSION = array();
		session_decode($dat);
	}

	return array($id, $shared_secret);
}


function new_secret () {
	$r = '';
	for($i=0; $i<20; $i++)
		$r .= chr(mt_rand(0, 255));

	debug("Generated new secret. Size: " . strlen($r));
	return $r;
}


function random ( $max ) {
	if (strlen($max) < 4)
		return mt_rand(1, $max - 1);

	$r = '';
	for($i=1; $i<strlen($max) - 1; $i++)
		$r .= mt_rand(0,9);
	$r .= mt_rand(1,9);

	return $r;
}


function secret ( $handle ) {
	if (! preg_match('/^\w+$/', $handle))
		return array(false, 0);

	if (is_array($_SESSION)) {
		$sid = session_id();
		$dat = session_encode();
		session_write_close();
	}

	session_id($handle);
	session_start();
	debug('Started session to acquire key: ' . session_id());

	$secret = session_is_registered('shared_secret')
		? base64_decode($_SESSION['shared_secret'])
		: false;

	$expiration = session_is_registered('expiration')
		? $_SESSION['expiration']
		: null;

	session_write_close();

	if (isset($sid)) {
		session_id($sid);
		session_start();
		$_SESSION = array();
		session_decode($dat);
	}

	debug("Session expires in: '$expiration', key length: " . strlen($secret));
	return array($secret, $expiration);
}


function self_check () {
	global $profile, $sreg;

	if (! isset($profile) || ! is_array($profile)) {
		if (! @include('MyID.config.php'))
			error_500('Configuration is missing.<br/>Default realm: phpMyID' . (ini_get('safe_mode') ? '-' . getmyuid() : ''));
	}

	if (version_compare(phpversion(), '4.2.0', 'lt'))
		error_500('The required minimum version of PHP is 4.2.0, you are running ' . phpversion());

	$extensions = array('session', 'pcre');
	foreach ($extensions as $x) {
		if (! extension_loaded($x))
			@dl($x);
		if (! extension_loaded($x))
			error_500("Required extension '$x' is missing.");
	}

	$keys = array('auth_username', 'auth_password');
	foreach ($keys as $key) {
		if (! array_key_exists($key, $profile))
			error_500("'$key' is missing from your profile.");
	}

	if (! isset($sreg) || ! is_array($sreg))
		$sreg = array();
}


// Borrowed from PHP-OpenID; http://openidenabled.com
function sha1_20 ($v) {
	if (version_compare(phpversion(), '5.0.0', 'ge'))
		return sha1($v, true);

	$hex = sha1($v);
	$r = '';
	for ($i = 0; $i < 40; $i += 2) {
		$hexcode = substr($hex, $i, 2);
		$charcode = base_convert($hexcode, 16, 10);
		$r .= chr($charcode);
	}
	return $r;
}


if (! function_exists('sys_get_temp_dir') && ini_get('open_basedir') == false) {
function sys_get_temp_dir () {
	$keys = array('TMP', 'TMPDIR', 'TEMP');
	foreach ($keys as $key) {
		if (isset($_ENV[$key]) && is_dir($_ENV[$key]) && is_writable($_ENV[$key]))
			return realpath($_ENV[$key]);
	}

	$tmp = tempnam(false, null);
	if (file_exists($tmp)) {
		$dir = realpath(dirname($tmp));
		unlink($tmp);
		return realpath($dir);
	}

	return realpath(dirname(__FILE__));
}} elseif (! function_exists('sys_get_temp_dir')) {
function sys_get_temp_dir () {
	return realpath(dirname(__FILE__));
}}


function user_session () {
	global $proto, $profile;

	session_name('phpMyID_Server');
	@session_start();

	$profile['authorized'] = (isset($_SESSION['auth_username'])
			    && $_SESSION['auth_username'] == $profile['auth_username'])
			? true
			: false;

	debug('Started user session: ' . session_id() . ' Auth? ' . $profile['authorized']);
}


function wrap_html ( $message ) {
	global $profile;

	header('Content-Type: text/html; charset=UTF-8');
	echo '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html>
<head>
<title>phpMyID</title>
<link rel="openid.server" href="' . $profile['req_url'] . '" />
<link rel="openid.delegate" href="' . $profile['idp_url'] . '" />
</head>
<body>
<p>' . $message . '</p>
</body>
</html>
';

	exit(0);
}


function wrap_kv ( $keys ) {
	debug($keys, 'Wrapped key/vals');
	header('Content-Type: text/plain; charset=UTF-8');
	foreach ($keys as $key => $value)
		printf("%s:%s\n", $key, $value);

	exit(0);
}


function wrap_location ($url, $keys) {
	$keys = append_openid($keys);
	debug($keys, 'Location keys');

	$q = strpos($url, '?') ? '&' : '?';
	header('Location: ' . $url . $q . http_build_query($keys));
	debug('Location: ' . $url . $q . http_build_query($keys));
	exit(0);
}


function wrap_refresh ($url) {
	header('Content-Type: text/html; charset=UTF-8');
	echo '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html>
<head>
<title>phpMyID</title>
<meta http-equiv="refresh" content="0;url=' . $url . '">
</head>
<body>
<p>Redirecting to <a href="' . $url . '">' . $url . '</a></p>
</body>
</html>
';

	debug('Refresh: ' . $url);
	exit(0);
}


function x_or ($a, $b) {
	$r = "";

	for ($i = 0; $i < strlen($b); $i++)
		$r .= $a[$i] ^ $b[$i];
	debug("Xor size: " . strlen($r));
	return $r;
}



/*
 * App Initialization
 */

// Do a check to be sure everything is set up correctly
self_check();


// Set any remaining profile values
$port = ((isset($_SERVER["HTTPS"]) && $_SERVER["HTTPS"] == 'on' && $_SERVER['SERVER_PORT'] == 443)
	  || $_SERVER['SERVER_PORT'] == 80)
		? ''
		: ':' . $_SERVER['SERVER_PORT'];

$proto = (isset($_SERVER["HTTPS"]) && $_SERVER["HTTPS"] == 'on') ? 'https' : 'http';

if (! array_key_exists('idp_url', $profile))
	$profile['idp_url'] = sprintf("%s://%s%s%s",
			      $proto,
			      $_SERVER['SERVER_NAME'],
			      $port,
			      $_SERVER['PHP_SELF']);

$profile['req_url'] = sprintf("%s://%s%s%s",
		      $proto,
		      $_SERVER['HTTP_HOST'],
		      $port,
		      $_SERVER["REQUEST_URI"]);

if (! array_key_exists('auth_domain', $profile))
	$profile['auth_domain'] = $profile['req_url'] . ' ' . $profile['idp_url'];

if (! array_key_exists('auth_realm', $profile))
	$profile['auth_realm'] = 'phpMyID';

if (! array_key_exists('lifetime', $profile))
	$profile['lifetime'] = ((session_cache_expire() * 60) - 10);

if (! array_key_exists('logfile', $profile))
	$profile['logfile'] = sys_get_temp_dir() . DIRECTORY_SEPARATOR . $profile['auth_realm'] . '.debug.log';


// These are used internally and cannot be overridden
$profile['authorized'] = false;

$profile['php_realm'] = $profile['auth_realm'] . (ini_get('safe_mode') ? '-' . getmyuid() : '');


// Decide which runmode, based on user request or default
$run_mode = (isset($_REQUEST['openid_mode'])
	  && in_array($_REQUEST['openid_mode'], $known['openid_modes']))
	? $_REQUEST['openid_mode']
	: 'no';


// Run in the determined runmode
debug("Run mode: $run_mode at: " . time());
debug($_REQUEST, 'Request params');
eval($run_mode . '_mode();');
?>
