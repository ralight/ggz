--- SQL database format for the GGZ Gaming Zone
--- Optional tables: rankings pages

CREATE TABLE userinfo (
    id serial NOT NULL,
    handle character varying(256),
    photo character varying(256),
    gender character varying(256),
    country character varying(256),
    pubkey character varying(8192),
	blogfeed character varying(256),
	longitude double precision,
	latitude double precision
);

CREATE TABLE "teams" (
	"teamname" character varying(256),
	"fullname" character varying(256),
	"icon" character varying(256),
	"foundingdate" integer,
	"founder" character varying(256),
	"homepage" character varying(256)
);

CREATE TABLE "teammembers" (
	"teamname" character varying(256),
	"handle" character varying(256),
    "role" character varying(256),
	"entrydate" integer
);

CREATE TABLE "placements" (
	"tournament" integer,
	"place" integer,
	"handle" character varying(256)
);

CREATE TABLE "rankings" (
	"game" character varying(256),
	"method" character varying(256)
);

CREATE TABLE "auth" (
	"handle" character varying(256),
	"cookie" character varying(256)
);

CREATE TABLE "karma" (
	"fromhandle" character varying(256),
	"tohandle" character varying(256),
	"karma" integer
);
