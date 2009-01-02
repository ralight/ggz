--- SQL database format for the GGZ Gaming Zone
--- Add indices etc. as you like :)

-- FIXME: this schema needs to be improved based on mysql_schema.sql
-- FIXME: (and vice-versa)

CREATE TABLE "users" (
	"id" serial NOT NULL,
	"handle" character varying(256) NOT NULL,
	"password" character varying(256),
	"name" character varying(256),
	"email" character varying(256),
	"lastlogin" bigint,
	"permissions" integer default 7,
	"firstlogin" bigint,
	"confirmed" boolean
);

CREATE TABLE "userinfo" (
	"id" serial NOT NULL,
	"user_id" integer NOT NULL,
	"photo" text NOT NULL,
	"gender" text NOT NULL,
	"country" text NOT NULL,
	"pubkey" text NOT NULL,
	"blogfeed" text NOT NULL,
	"longitude" double NOT NULL default '0',
	"latitude" double NOT NULL default '0',
	"alterpass" text NOT NULL
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

CREATE TABLE "matches" (
	"id" serial NOT NULL,
	"date" bigint,
	"game" character varying(256),
	"winner" character varying(256),
	"savegame" character varying(256)
);

CREATE TABLE "matchplayers" (
	"id" serial NOT NULL,
	"match" bigint,
	"handle" character varying(256),
	"playertype" character varying(256)
);

CREATE TABLE "savegames" (
	"id" serial NOT NULL,
	"date" bigint,
	"game" character varying(256),
	"owner" character varying(256),
	"savegame" character varying(256),
	"tableid" bigint,
	"stamp" bigint
);

CREATE TABLE "savegameplayers" (
	"id" serial NOT NULL,
	"tableid" bigint,
	"stamp" bigint,
	"seat" bigint,
	"handle" character varying(256),
	"seattype" character varying(256)
);

CREATE TABLE "teammembers" (
	"id" serial NOT NULL,
	"teamname" text NOT NULL,
	"username" text NOT NULL,
	"role" text NOT NULL,
	"entrydate" bigint NOT NULL
);

CREATE TABLE "teams" (
	"id" serial NOT NULL,
	"teamname" text NOT NULL,
	"fullname" text NOT NULL,
	"icon" text NOT NULL,
	"foundingdate" bigint NOT NULL,
	"founder" text NOT NULL,
	"homepage" text NOT NULL
);

CREATE TABLE "tournamentplayers" (
	"id" serial NOT NULL,
	"tid" bigint NOT NULL,
	"number" bigint NOT NULL,
	"name" text NOT NULL,
	"playertype" text NOT NULL
);

CREATE TABLE "tournaments" (
	"id" serial NOT NULL,
	"name" text NOT NULL,
	"game" text NOT NULL,
	"date" bigint NOT NULL,
	"organizer" text NOT NULL
);
