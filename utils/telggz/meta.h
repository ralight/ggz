#ifndef META_H
#define META_H

struct serverggz_t
{
	int id;
	char *host;
	int port;
	int preference;
	char *version;
	char *location;
	int speed;
};
typedef struct serverggz_t ServerGGZ;

/* Initialize the meta server configuration */
void meta_init();

/* Get a list of GGZ servers */
ServerGGZ **meta_query(const char *version);

/* Update internal list of meta servers */
void meta_sync();

/* Free a ServerGGZ list */
void meta_free(ServerGGZ **server);

#endif

