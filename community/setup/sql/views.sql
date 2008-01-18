--- SQL database format for the GGZ Gaming Zone
--- Optional views: player permissions

CREATE VIEW permissions AS
SELECT users.handle,
((users.permissions & 001) >> 0) AS join_table,
((users.permissions & 002) >> 1) AS launch_table,
((users.permissions & 004) >> 2) AS rooms_login,
((users.permissions & 008) >> 3) AS rooms_admin,
((users.permissions & 016) >> 4) AS chat_announce,
((users.permissions & 032) >> 5) AS chat_bot,
((users.permissions & 064) >> 6) AS no_stats,
((users.permissions & 128) >> 7) AS edit_tables,
((users.permissions & 256) >> 8) AS table_privmsg
FROM users;

CREATE VIEW permissionmasks AS
SELECT users.handle,
((users.permissions & (008 + 016 + 128 + 256)) = 008 + 016 + 128 + 256) AS admin_mask,
((users.permissions & (128 + 256)) = 128 + 256) AS host_mask,
((users.permissions & (001 + 002 + 004)) = 001 + 002 + 004) AS normal_mask,
((users.permissions & (032)) = 032) AS bot_mask,
((users.permissions & (001 + 002 + 064)) = 001 + 002 + 064) AS anon_mask
FROM users;

