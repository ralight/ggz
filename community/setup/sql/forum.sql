--- SQL database format for the GGZ Gaming Zone
--- Optional tables: integrated GGZ Community forum

CREATE TABLE forums (
	"id" serial NOT NULL,
	"name" character varying(256),
	"club" character varying(256)
);

CREATE TABLE forumthreads (
	"id" serial NOT NULL,
	"forum" bigint,
	"thread" bigint,
	"title" character varying(256)
);

CREATE TABLE forumthreadmessages (
	"id" serial NOT NULL,
	"thread" bigint,
	"message" bigint
);

CREATE TABLE forummessages (
	"id" serial NOT NULL,
	"sender" character varying(256),
	"receiver" character varying(256),
	"date" bigint,
	"read" boolean,
	"title" character varying(256),
	"message" text
);

