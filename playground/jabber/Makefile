CPPFLAGS = -Wall -O2
CXX = g++

LIBGGZ_ARGS = -I /home/josef/ggz/installation/include -L /home/josef/ggz/installation/lib -lggz
GGZCORE_ARGS = -I /home/josef/ggz/installation/include -L /home/josef/ggz/installation/lib -lggzcore

all:
	$(CXX) -o ggzd-jabberproxy ggzd-jabberproxy.cc -lgloox $(LIBGGZ_ARGS) $(GGZCORE_ARGS)

