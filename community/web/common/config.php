<?php

global $config_object;

class Config
{
	var $config;
	
	function Config($file)
	{
		global $communityname;
		global $communityslogan;
		global $communitycopyright_title;
		global $communitycopyright_link;
		global $communitytls;
		global $communityurl;
		global $communitymail;

		global $dbhost;
		global $dbname;
		global $dbuser;
		global $dbpass;
		global $dbtype;

		global $ggzhost;
		global $ggzname;
		global $ggzversion;
		global $ggzgamedir;

		require_once($file);

		$this->config = array();

		$this->config['name'] = $communityname;
		$this->config['slogan'] = $communityslogan;
		$this->config['copyright_title'] = $communitycopyright_title;
		$this->config['copyright_link'] = $communitycopyright_link;
		$this->config['tls'] = $communitytls;
		$this->config['url'] = $communityurl;
		$this->config['mail'] = $communitymail;

		$this->config['dbhost'] = $dbhost;
		$this->config['dbname'] = $dbname;
		$this->config['dbuser'] = $dbuser;
		$this->config['dbpass'] = $dbpass;
		$this->config['dbtype'] = $dbtype;

		$this->config['ggzhost'] = $ggzhost;
		$this->config['ggzname'] = $ggzname;
		$this->config['ggzversion'] = $ggzversion;
		$this->config['ggzgamedir'] = $ggzgamedir;
	}

	function getvalue($key)
	{
		global $config_object;

		return $config_object->config[$key];
	}

	function put($key)
	{
		echo Config::getvalue($key);
	}
}

$config_object = new Config("htconf");

?>
