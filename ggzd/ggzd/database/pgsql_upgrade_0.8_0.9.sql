--- GGZ Gaming Zone database schema upgrade from version 0.8 to version 0.9
 
CREATE TABLE "userinfo" (
	"id" serial NOT NULL,
	"handle" varchar(256) NOT NULL,
	"photo" text NOT NULL,
	"gender" text NOT NULL,
	"country" text NOT NULL,
	"pubkey" text NOT NULL,
	"blogfeed" text NOT NULL,
	"longitude" double NOT NULL default '0',
	"latitude" double NOT NULL default '0',
	"alterpass" text NOT NULL
);

create table "teammembers" (
	"id" serial not null,
	"teamname" text not null,
	"username" text not null,
	"role" text not null,
	"entrydate" bigint not null
);

create table "teams" (
	"id" serial not null,
	"teamname" text not null,
	"fullname" text not null,
	"icon" text not null,
	"foundingdate" bigint not null,
	"founder" text not null,
	"homepage" text not null
);

create table "tournamentplayers" (
	"id" serial not null,
	"tid" bigint not null,
	"number" bigint not null,
	"name" text not null,
	"playertype" text not null
);

create table "tournaments" (
	"id" serial not null,
	"name" text not null,
	"game" text not null,
	"date" bigint not null,
	"organizer" text not null
);

UPDATE control SET value = '0.9' WHERE value = '0.8';

