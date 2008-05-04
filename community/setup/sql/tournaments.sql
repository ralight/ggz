--- SQL database format for the GGZ Gaming Zone
--- Optional tables: tournaments pages

CREATE TABLE tournaments (
	"id" serial NOT NULL,
	"name" character varying(256),
	"game" character varying(256),
	"room" character varying(256),
	"date" bigint,
	"organizer" character varying(256)
);

CREATE TABLE tournamentplayers (
	"id" bigint,
	"number" bigint,
	"name" character varying(256),
	"playertype" character varying(256)
);

CREATE TABLE rooms (
	"id" serial NOT NULL,
	"filebased" boolean,
	"name" character varying(256),
	"description" character varying(256),
	"gametype" character varying(256),
	"maxplayers" bigint,
	"maxtables" bigint,
	"entryrestriction" character varying(256),
	"action" character varying(256)
);

CREATE RULE ggzroomchange AS
ON INSERT TO rooms DO NOTIFY ggzroomchange;

CREATE RULE ggzroomchangedel AS
ON UPDATE TO rooms DO NOTIFY ggzroomchange;

