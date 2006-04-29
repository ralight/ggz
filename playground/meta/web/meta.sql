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
kamikaze	Kamikaze	kamikaze.png	http://kamikaze.coolprojects.org/	\N
widelands	Widelands	widelands.png	http://www.widelands.org/	\N
freelords	Freelords	freelords.png	http://www.freelords.org/	\N
chess	GGZ Chess	chess.png	http://www.ggzgamingzone.org/engines/chess/	\N
geekgame	GGZ Geekgame	geekgame.png	http://www.ggzgamingzone.org/engines/geekgame/	\N
keepalive	GGZ Keepalive	keepalive.png	http://www.ggzgamingzone.org/engines/keepalive/	\N
cchess	GGZ Chinese Chess	cchess.png	http://www.ggzgamingzone.org/engines/cchess/	\N
dots	GGZ Connect The Dots	dots.png	http://www.ggzgamingzone.org/engines/dots/	\N
tictactoe	GGZ Tic-Tac-Toe	tictactoe.png	http://www.ggzgamingzone.org/engines/tictactoe/	\N
hastings	GGZ Hastings1066	hastings.png	http://www.ggzgamingzone.org/engines/hastings/	\N
netspades	GGZ NetSpades	netspades.png	http://www.ggzgamingzone.org/engines/netspades/	\N
ccheckers	GGZ Chinese Checkers	ccheckers.png	http://www.ggzgamingzone.org/engines/ccheckers/	\N
reversi	GGZ Reversi	reversi.png	http://www.ggzgamingzone.org/engines/reversi/	\N
escape	GGZ Escape	escape.png	http://www.ggzgamingzone.org/engines/escape/	\N
krosswater	GGZ Krosswater	krosswater.png	http://www.ggzgamingzone.org/engines/krosswater/	\N
combat	GGZ Combat	combat.png	http://www.ggzgamingzone.org/engines/combat/	\N
lapocha	GGZCards LaPocha	ggzcards.png	http://www.ggzgamingzone.org/engines/lapocha/	\N
bridge	GGZCards Bridge	ggzcards.png	http://www.ggzgamingzone.org/engines/bridge/	\N
fortytwo	GGZCards FortyTwo	ggzcards.png	http://www.ggzgamingzone.org/engines/fortytwo/	\N
hearts	GGZCards Hearts	ggzcards.png	http://www.ggzgamingzone.org/engines/hearts/	\N
suaro	GGZCards Suaro	ggzcards.png	http://www.ggzgamingzone.org/engines/suaro/	\N
sueca	GGZCards Sueca	ggzcards.png	http://www.ggzgamingzone.org/engines/sueca/	\N
whist	GGZCards Whist	ggzcards.png	http://www.ggzgamingzone.org/engines/whist/	\N
muehle	GGZ Muehle	muehle.png	http://www.ggzgamingzone.org/engines/muehle/	\N
arimaa	GGZBoard Arimaa	hnefatafl.png	http://www.ggzgamingzone.org/engines/arimaa/	\N
shogi	GGZBoard Shogi	hnefatafl.png	http://www.ggzgamingzone.org/engines/shogi/	\N
ludo	GGZBoard Ludo	hnefatafl.png	http://www.ggzgamingzone.org/engines/ludo/	\N
hnefatafl	GGZBoard Hnefatafl	hnefatafl.png	http://www.ggzgamingzone.org/engines/hnefatafl/	\N
checkers	GGZBoard Checkers	hnefatafl.png	http://www.ggzgamingzone.org/engines/checkers/	\N
go	GGZ Go	go.png	http://www.ggzgamingzone.org/engines/go/	\N
connectx	GGZ ConnectX	connectx.png	http://www.ggzgamingzone.org/engines/connectx/	\N
monopd	Monopd/Atlantik		http://unixcode.org/monopd/	\N
\.

-- Some convenience data for GGZ games
COPY zones ("key", game) FROM stdin;
ggz	muehle
ggz	reversi
ggz	chess
ggz	geekgame
ggz	keepalive
ggz	cchess
ggz	dots
ggz	tictactoe
ggz	hastings
ggz	netspades
ggz	ccheckers
ggz	escape
ggz	krosswater
ggz	combat
ggz	lapocha
ggz	bridge
ggz	fortytwo
ggz	hearts
ggz	suaro
ggz	sueca
ggz	whist
ggz	arimaa
ggz	shogi
ggz	ludo
ggz	hnefatafl
ggz	checkers
ggz	go
ggz	connectx
ggz	kamikaze
ggz	freeciv
ggz	teg
\.

