--- SQL database format for the GGZ Gaming Zone
--- Add indices etc. as you like :)

CREATE TABLE "users" (
	"id" serial NOT NULL,
	"handle" character varying(256) NOT NULL,
	"password" character varying(256),
	"name" character varying(256),
	"email" character varying(256),
	"lastlogin" bigint,
	"permissions" integer,
	"firstlogin" bigint,
	"confirmed" boolean
);

CREATE TABLE "stats" (
	"id" serial NOT NULL,
	"handle" character varying(256),
	"game" character varying(256),
	"wins" bigint,
	"losses" bigint,
	"ties" bigint,
	"forfeits" bigint,
	"rating" double precision,
	"ranking" bigint,
	"highscore" bigint
);

CREATE TABLE "control" (
	"key" character varying(256),
	"value" character varying(256)
);

CREATE TABLE matches (
	"id" serial NOT NULL,
	"date" bigint,
	"game" character varying(256),
	"winner" character varying(256),
	"savegame" character varying(256)
);

CREATE TABLE matchplayers (
	"id" serial NOT NULL,
	"match" bigint,
	"handle" character varying(256),
	"playertype" character varying(256)
);

CREATE TABLE savegames (
	"id" serial NOT NULL,
	"date" bigint,
	"game" character varying(256),
	"owner" character varying(256),
	"savegame" character varying(256)
);

