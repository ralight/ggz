--- SQL database format for the GGZ Gaming Zone

CREATE TABLE `users` (
	`id` INTEGER PRIMARY KEY AUTOINCREMENT,
	`handle` TEXT,
	`password` TEXT,
	`name` TEXT,
	`email` TEXT,
	`lastlogin` INT,
	`permissions` INT
);

CREATE TABLE `control` (
	`key` varchar(256),
	`value` varchar(256)
);

