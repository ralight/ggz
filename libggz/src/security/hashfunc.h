#ifndef GGZ_HASH_H
#define GGZ_HASH_H

/* Hash data structure */
typedef struct
{
	char *hash;
	int hashlen;
} hash_t;

/* Create a hash over a text, allocating space as needed */
hash_t ggz_hash_create(const char *algo, const char *text);

/* Create a HMAC hash over a text, allocating space as needed */
hash_t ggz_hmac_create(const char *algo, const char *text, const char *secret);

#endif

