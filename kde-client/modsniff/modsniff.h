#ifndef MODSNIFF_H
#define MODSNIFF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize the module sniffer */
/* Returns 0 on success, -1 on error */
int modsniff_init();

/* Retreive a list of new modules. Must NOT be free()'d! */
/* Returns null-terminated games list on success, null on error */
char **modsniff_list();

/* Merges the list with a list of (currently visible) installed modules */
/* Returns null-terminated char* pointer list, null on error */
char **modsniff_merge(char **orig);

/* Finish and free memory. */
/* Returns 0 on success, -1 on error */
int modsniff_end();

#ifdef __cplusplus
}
#endif

#endif

