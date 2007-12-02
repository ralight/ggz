CPPFLAGS = -Wall -O2
CXX = g++

LIBGGZ_ARGS = -I /home/josef/ggz/installation/include -L /home/josef/ggz/installation/lib -lggz
GGZCORE_ARGS = -I /home/josef/ggz/installation/include -L /home/josef/ggz/installation/lib -lggzcore

#sources = game.cc ggz-jabber.cc loop.cc server.cc
sources = ggz-jabber.cc

all:
	$(CXX) -o ggz-jabber $(sources) -lgloox $(LIBGGZ_ARGS) $(GGZCORE_ARGS)

