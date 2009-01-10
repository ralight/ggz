CPPFLAGS = `pkg-config --cflags ggzcore`
LDFLAGS = `pkg-config --libs ggzcore`

all:
	gcc -Wall -Werror -O2 -g -o ggzcoreconn ggzcoreconn.c ggzcore_mainloop.c $(CPPFLAGS) $(LDFLAGS)

clean:
	rm -f ggzcoreconn

