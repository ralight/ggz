-- Maintained by hand
CREATE TABLE games (
    "key" character varying(256),
    name character varying(256),
    logo character varying(256),
    homepage character varying(256),
    "zone" character varying(256)
);

-- Filled automatically by aggregating process
-- Based on configuration file
CREATE TABLE metaservers (
    "key" character varying(256),
    url character varying(256)
);

-- Filled automatically by aggregating process
-- Based on results from queries
CREATE TABLE gameservers (
    "key" character varying(256),
    uri character varying(256),
    title character varying(256),
    "version" character varying(256),
    state character varying(256),
    topic character varying(256),
    players integer,
    available integer
);

-- Filled automatically by aggregating process
-- Based on results from queries
-- Used only for games which have zone set
CREATE TABLE zones (
    "key" character varying(256),
    game character varying(256)
);

-- Some convenience data (but you need the logos!)
COPY games ("key", name, logo, homepage, "zone") FROM stdin;
freeciv	Freeciv	freeciv.png	http://www.freeciv.org/	\N
teg	T.E.G.	teg.png	http://teg.sourceforge.net/	\N
liquidwar	LiquidWar		http://www.ufoot.org/	\N
worldforge	WorldForge		http://www.worldforge.org/	\N
ggz	GGZ Community	ggz.png	http://www.ggzcommunity.org/	ggz
reversi	Reversi	reversi.png	http://www.ggzgamingzone.org/engines/reversi/	\N
muehle	Muehle	muehle.png	http://www.ggzgamingzone.org/engines/muehle/	\N
ggzcards-spades	GGZCards Spades	ggzcards.png	http://www.ggzgamingzone.org/engines/ggzcards-spades/	\N
\.

-- Some convenience data for GGZ games
COPY zones ("key", game) FROM stdin;
ggz	ggzcards-spades
ggz	muehle
ggz	reversi
\.

