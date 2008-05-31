<?php

global $config_object;

class Config
{
	var $config;
	var $theme;
	var $unconfigured;
	
	function Config($file)
	{
		global $communityname;
		global $communityslogan;
		global $communitycopyright_title;
		global $communitycopyright_link;
		global $communitytls;
		global $communityurl;
		global $communitymail;
		global $communityregistration;

		global $dbhost;
		global $dbname;
		global $dbuser;
		global $dbpass;
		global $dbtype;
		global $dbport;

		global $ggzhost;
		global $ggzurl;
		global $ggzname;
		global $ggzversion;
		global $ggzgamedir;
		global $ggzconfigfile;

		global $communitytheme;

		$ret = @include_once($file);
		if (!$ret) :
			$this->config = array();
			$this->theme = "default";
			$this->unconfigured = true;
			return;
		endif;

		if ($ggzconfigfile) :
			$ini = $this->parseconfigfile($ggzconfigfile);
			if ($ini) :
				$dbhost = $ini["General"]["DatabaseHost"];
				$dbport = $ini["General"]["DatabasePort"];
				$dbname = $ini["General"]["DatabaseName"];
				$dbuser = $ini["General"]["DatabaseUsername"];
				$dbpass = $ini["General"]["DatabasePassword"];
				$dbtype = $ini["General"]["DatabaseType"];
				$ggzname = $ini["General"]["ServerName"];
				$ggzmail = $ini["General"]["AdminEmail"];
				$communityregistration = $ini["General"]["RegistrationPolicy"];

				if (!$communityname) :
					$communityname = $ggzname;
				endif;
				if (!$communitymail) :
					$communitymail = $ggzmail;
				endif;
			endif;
		endif;

		if (!$communityregistration) :
			$communityregistration = "open";
		endif;

		if (!$dbport) :
			$dbport = 5432;
		endif;

		$this->config = array();

		$this->config['name'] = $communityname;
		$this->config['slogan'] = $communityslogan;
		$this->config['copyright_title'] = $communitycopyright_title;
		$this->config['copyright_link'] = $communitycopyright_link;
		$this->config['tls'] = $communitytls;
		$this->config['url'] = $communityurl;
		$this->config['mail'] = $communitymail;
		$this->config['registration'] = $communityregistration;

		$this->config['dbhost'] = $dbhost;
		$this->config['dbport'] = $dbport;
		$this->config['dbname'] = $dbname;
		$this->config['dbuser'] = $dbuser;
		$this->config['dbpass'] = $dbpass;
		$this->config['dbtype'] = $dbtype;

		$this->config['ggzhost'] = $ggzhost;
		$this->config['ggzurl'] = $ggzurl;
		$this->config['ggzname'] = $ggzname;
		$this->config['ggzversion'] = $ggzversion;
		$this->config['ggzgamedir'] = $ggzgamedir;
		$this->config['ggzconfigfile'] = $ggzconfigfile;

		if (!$communitytheme) :
			$communitytheme = "default";
		endif;

		$this->theme = $communitytheme;

		$this->features = $features;
	}

	function parseconfigfile($configfile)
	{
		$f = @fopen($configfile, "r");
		if(!$f) :
			return null;
		endif;

		$section = "";
		$content = array();
		while(!feof($f))
		{
			$a = fgets($f);
			if ($a == "\n") continue;
			$a = substr($a, 0, strlen($a) - 1);
			if ($a[0] == "#") continue;
			if (($a[0] == "[") and ($a[strlen($a) - 1] == "]")) :
				$section = substr($a, 1, strlen($a) - 2);
				continue;
			endif;
			#echo "//$section// $a<br>";
			$entry = explode("=", $a);
			$key = trim($entry[0]);
			$value = trim($entry[1]);
			#echo "::$key::$value::<br>";
			$content[$section][$key] = $value;
		}
		fclose($f);

		return $content;
	}

	function feature($feature)
	{
		global $config_object;

		if ($config_object->features) :
			return $config_object->features[$feature];
		else :
			return true;
		endif;
	}

	function getvalue($key)
	{
		global $config_object;

		$preval = $config_object->config[$key];

		if (is_array($preval)) :
			$val = $preval[Locale::language()];
			if (!$val) :
				$val = $preval[''];
			endif;
		else :
			$val = $preval;
		endif;

		return $val;
	}

	function put($key)
	{
		echo Config::getvalue($key);
	}

	function gettheme($image)
	{
		global $config_object;

		return "/images/" . $config_object->theme . "/$image";
	}

	function theme($image)
	{
		echo Config::gettheme($image);
	}
}

$config_object = new Config("htconf");

?>
