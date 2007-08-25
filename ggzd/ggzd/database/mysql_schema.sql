
CREATE TABLE `users` (
	`id` int(11) NOT NULL auto_increment,
	`handle` varchar(25) NOT NULL default '',
	`password` varchar(32) NOT NULL default '',
	`name` varchar(255) NOT NULL default '',
	`email` varchar(255) NOT NULL default '',
	`firstlogin` timestamp NOT NULL default '0',
	`lastlogin` timestamp NOT NULL default '0',
	`perms` int(11) NOT NULL default '7',
	PRIMARY KEY  (`id`),
	UNIQUE KEY `handle` (`handle`),
	KEY `player_get_first` (`password`,`name`,`email`,`lastvisit`,`perms`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `userinfo` (
	`id` int(11) NOT NULL auto_increment,
	`user_id` int(11) NOT NULL,
	`photo` text NOT NULL,
	`gender` text NOT NULL,
	`country` text NOT NULL,
	`pubkey` text NOT NULL,
	`blogfeed` text NOT NULL,
	`longitude` double NOT NULL default '0',
	`latitude` double NOT NULL default '0',
	`alterpass` text NOT NULL,
	PRIMARY KEY  (`id`),
	CONSTRAINT `user_id` FOREIGN KEY (`user_id`) REFERENCES `users` (`id`) ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `stats` (
	`id` int(11) NOT NULL auto_increment,
	`user_id` int(11) NOT NULL default '0',
	`game` text NOT NULL,
	`wins` int(11) NOT NULL default '0',
	`losses` int(11) NOT NULL default '0',
	`ties` int(11) NOT NULL default '0',
	`forfeits` int(11) NOT NULL default '0',
	`rating` double NOT NULL default '0',
	`ranking` int(11) NOT NULL default '0',
	`highscore` int(11) NOT NULL default '0',
	PRIMARY KEY  (`id`),
	KEY `stats_lookup` (`username`(30),`game`(30)),
	CONSTRAINT `userid` FOREIGN KEY (`user_id`) REFERENCES `users` (`id`) ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `matches` (
	`id` int(11) NOT NULL auto_increment,
	`date` int(11) NOT NULL default '0',
	`game` text NOT NULL,
	`winner` text NOT NULL,
	`savegame` text,
	PRIMARY KEY  (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `matchplayers` (
	`id` int(11) NOT NULL auto_increment,
	`match_id` int(11) NOT NULL default '0',
	`username` text NOT NULL,
	`playertype` text NOT NULL,
	PRIMARY KEY  (`id`),
	CONSTRAINT `match_id` FOREIGN KEY (`match_id`) REFERENCES `matches` (`id`) ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `savegames` (
	`id` int(11) NOT NULL auto_increment,
	`date` int(11) NOT NULL default '0',
	`game` text NOT NULL,
	`owner` text NOT NULL,
	`savegame` text NOT NULL,
	PRIMARY KEY  (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `teammembers` (
	`id` int(11) NOT NULL auto_increment,
	`team_id` int(11) NOT NULL default '0',
	`user_id` int(11) NOT NULL default '0',
	`teamname` text NOT NULL,
	`username` text NOT NULL,
	`role` text NOT NULL,
	`entrydate` int(11) NOT NULL default '0',
	PRIMARY KEY  (`id`),
	CONSTRAINT `user_id` FOREIGN KEY (`user_id`) REFERENCES `users` (`id`) ON UPDATE CASCADE ON DELETE CASCADE
	CONSTRAINT `team_id` FOREIGN KEY (`team_id`) REFERENCES `teams` (`id`) ON UPDATE CASCADE ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `teams` (
	`id` int(11) NOT NULL auto_increment,
	`teamname` text NOT NULL,
	`fullname` text NOT NULL,
	`icon` text NOT NULL,
	`foundingdate` int(11) NOT NULL default '0',
	`founder` text NOT NULL,
	`homepage` text NOT NULL
	PRIMARY KEY  (`id`),
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `tournamentplayers` (
	`id` int(11) NOT NULL auto_increment,
	`tid` int(11) NOT NULL default '0',
	`number` int(11) NOT NULL default '0',
	`name` text NOT NULL,
	`playertype` text NOT NULL,
	PRIMARY KEY  (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `tournaments` (
	`id` int(11) NOT NULL auto_increment,
	`name` text NOT NULL,
	`game` text NOT NULL,
	`date` int(11) NOT NULL default '0',
	`organizer` text NOT NULL,
	PRIMARY KEY  (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


