<?php
	# Mantis - a php based bugtracking system
	# Copyright (C) 2000 - 2002  Kenzaburo Ito - kenito@300baud.org
	# Copyright (C) 2002 - 2003  Mantis Team   - mantisbt-dev@lists.sourceforge.net
	# This program is distributed under the terms and conditions of the GPL
	# See the README and LICENSE files for details

	# --------------------------------------------------------
	# $Id: config_inc.php 5980 2004-04-24 17:27:42Z josef $
	# --------------------------------------------------------
	
	# This sample file contains the essential files that you MUST
	# configure to your specific settings.  You may override settings
	# from config_defaults_inc.php by assigning new values in this file

	# Rename this file to config_inc.php after configuration.

	###########################################################################
	# CONFIGURATION VARIABLES
	###########################################################################

	# In general a value of 0 means the feature is disabled and 1 means the
	# feature is enabled.  Any other cases will have an explanation.

	# Look in configuration.html or config_defaults_inc.php for more
	# detailed comments.

	# --- database variables ---------

	# set these values to match your setup
	$g_hostname      = "localhost";
	$g_port          = 3306;         # 3306 is default
	$g_db_username   = "*****";
	$g_db_password   = "*****";
	$g_database_name = "mantis";

	# --- email variables -------------
	$g_administrator_email  = "admin@ggzgamingzone.org";
	$g_webmaster_email      = "webmaster@ggzgamingzone.org";

	# the "From: " field in emails
	$g_from_email           = "ggz-bugs@mail.ggzgamingzone.org";

	# the "To: " address all emails are sent.  This can be a mailing list or archive address.
	# Actual users are emailed via the bcc: fields
	$g_to_email             = "ggz-bugs@mail.ggzgamingzone.org";

	# the return address for bounced mail
	$g_return_path_email    = "postmaster@ggzgamingzone.org";

	# --- login method ----------------
	# CRYPT or PLAIN or MD5 or LDAP or BASIC_AUTH
	$g_login_method = MD5;

	# --- using MS IIS ----------------
	# set to ON if you use IIS
	$g_use_iis = ON;

	# --- email vars ------------------
	# set to OFF to disable email check
	# These should be OFF for Windows installations
	$g_validate_email            = OFF;
	$g_check_mx_record           = OFF;

	# --- file upload settings --------
	# This is the master setting to disable *all* file uploading functionality
	#
	# The default value is ON but you must make sure file uploading is enabled
	#  in PHP as well.  You may need to add "file_uploads = TRUE" to your php.ini.
	$g_allow_file_upload	= ON;
?>
