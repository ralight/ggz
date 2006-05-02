#ifndef URI_H
#define URI_H

struct uri_t
{
	char *protocol;
	char *user;
	char *host;
	int port;
	char *path;
};
typedef struct uri_t URI;

URI uri_from_string(const char *uristring);
char *uri_to_string(URI uri);
void uri_free(URI uri);

#endif

