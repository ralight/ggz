#ifndef MODSNIFF_H
#define MODSNIFF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#ifndef strdup
  #define strdup(x) strcpy(((char*)malloc(strlen(x) + 1)), x)
#endif

/* Module structure */
struct _GGZModuleEntry
{
	char *name;
	char *frontend;
};

typedef struct _GGZModuleEntry GGZModuleEntry;
		
/* Initialize the module sniffer */
/* Returns 0 on success, -1 on error */
int modsniff_init(void);

/* Retreive a list of new modules. Must NOT be free()'d! */
/* Returns null-terminated games list on success, null on error */
GGZModuleEntry *modsniff_list(void);

/* Merges the list with a list of (currently visible) installed modules */
/* Returns null-terminated pointer list, null on error */
GGZModuleEntry *modsniff_merge(GGZModuleEntry *orig);

/* Finish and free memory. */
/* Returns 0 on success, -1 on error */
int modsniff_end(void);

#ifdef __cplusplus
}
#endif

#endif

