--- SQL database format for the GGZ Gaming Zone
--- Optional tables: tournaments pages

CREATE TABLE tournaments (
	"id" serial NOT NULL,
	"name" character varying(256),
	"game" character varying(256),
	"date" bigint,
	"organizer" character varying(256)
);

CREATE TABLE tournamentplayers (
	"id" bigint,
	"number" bigint,
	"name" character varying(256),
	"playertype" character varying(256)
);

