
CREATE TABLE `users` (
	`id` int(11) NOT NULL auto_increment,
	`handle` varchar(256) NOT NULL default '',
	`password` varchar(256) NOT NULL default '',
	`name` varchar(256) NOT NULL default '',
	`email` varchar(256) NOT NULL default '',
	`firstlogin` timestamp NOT NULL,
	`lastlogin` timestamp NOT NULL,
	`perms` int(11) NOT NULL default '7',
	`confirmed` boolean NOT NULL default 0,
	PRIMARY KEY (`id`),
	UNIQUE KEY `handle` (`handle`),
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `userinfo` (
	`id` int(11) NOT NULL auto_increment,
	`handle` varchar(256) NOT NULL,
	`photo` text NOT NULL,
	`gender` text NOT NULL,
	`country` text NOT NULL,
	`pubkey` text NOT NULL,
	`blogfeed` text NOT NULL,
	`longitude` double NOT NULL default '0',
	`latitude` double NOT NULL default '0',
	`alterpass` text NOT NULL,
	PRIMARY KEY (`id`),
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `stats` (
	`id` int(11) NOT NULL auto_increment,
	`handle` varchar(256),
	`game` text NOT NULL,
	`wins` int(11) NOT NULL default '0',
	`losses` int(11) NOT NULL default '0',
	`ties` int(11) NOT NULL default '0',
	`forfeits` int(11) NOT NULL default '0',
	`rating` double NOT NULL default '0',
	`ranking` int(11) NOT NULL default '0',
	`highscore` int(11) NOT NULL default '0',
	PRIMARY KEY (`id`),
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `control` (
	`key` varchar(256),
	`value` varchar(256)
);


CREATE TABLE `matches` (
	`id` int(11) NOT NULL auto_increment,
	`date` int(11) NOT NULL default '0',
	`game` text NOT NULL,
	`winner` varchar(256),
	`savegame` text,
	PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `matchplayers` (
	`id` int(11) NOT NULL auto_increment,
	`match` int(11) NOT NULL default '0',
	`handle` varchar(256),
	`playertype` varchar(256) NOT NULL,
	PRIMARY KEY (`id`),
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `savegames` (
	`id` int(11) NOT NULL auto_increment,
	`date` int(11) NOT NULL default '0',
	`game` varchar(256) NOT NULL,
	`owner` varchar(256) NOT NULL,
	`savegame` varchar(256) NOT NULL,
	`tableid` int(11) NOT NULL default '0',
	`stamp` int(11) NOT NULL default '0',
	PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `savegameplayers` (
	`id` int(11) NOT NULL auto_increment,
	`tableid` int(11) NOT NULL default '0',
	`stamp` int(11) NOT NULL default '0',
	`seat` int(11) NOT NULL default '0',
	`handle` varchar(256),
	`seattype` varchar(256) NOT NULL,
	PRIMARY KEY (`id`),
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `teammembers` (
	`id` int(11) NOT NULL auto_increment,
	`teamname` text NOT NULL,
	`username` text NOT NULL,
	`role` text NOT NULL,
	`entrydate` int(11) NOT NULL default '0',
	PRIMARY KEY (`id`),
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `teams` (
	`id` int(11) NOT NULL auto_increment,
	`teamname` text NOT NULL,
	`fullname` text NOT NULL,
	`icon` text NOT NULL,
	`foundingdate` int(11) NOT NULL default '0',
	`founder` text NOT NULL,
	`homepage` text NOT NULL
	PRIMARY KEY (`id`),
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `tournamentplayers` (
	`id` int(11) NOT NULL auto_increment,
	`tid` int(11) NOT NULL default '0',
	`number` int(11) NOT NULL default '0',
	`name` text NOT NULL,
	`playertype` text NOT NULL,
	PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


CREATE TABLE `tournaments` (
	`id` int(11) NOT NULL auto_increment,
	`name` text NOT NULL,
	`game` text NOT NULL,
	`date` int(11) NOT NULL default '0',
	`organizer` text NOT NULL,
	PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


