--- SQL database format for the GGZ Gaming Zone
--- Optional tables: rankings pages

CREATE TABLE userinfo (
    id serial NOT NULL,
    handle character varying(256),
    photo character varying(256),
    gender character varying(256),
    country character varying(256)
);

CREATE TABLE "teams" (
	"teamname" character varying(256),
	"fullname" character varying(256),
	"icon" character varying(256),
	"foundingdate" integer,
	"homepage" character varying(256)
);

CREATE TABLE "teammembers" (
	"teamname" character varying(256),
	"handle" character varying(256),
    role character varying(256)
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