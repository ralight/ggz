#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ggz.h>

#include "config.h"

#ifdef USE_GCRYPT
static int test16conversion(const char *orig, const char *hashing, const char *expected)
{
	hash_t hash;
	char *enc;
	int ret = 0;

	hash = ggz_hash_create(hashing, orig);

	enc = ggz_base16_encode(hash.hash, hash.hashlen);
	if(hash.hash) ggz_free(hash.hash);

	printf("Original: %s\n", orig);
	printf("Hashed:   %s\n", enc);
	printf("Expected: %s\n", expected);

	if(strcmp(enc, expected)) ret = 1;

	ggz_free(enc);

	return ret;
}

static int test64conversion(const char *orig, const char *hashing, const char *expected)
{
	hash_t hash;
	char *enc;
	int ret = 0;

	hash = ggz_hash_create(hashing, orig);

	enc = ggz_base64_encode(hash.hash, hash.hashlen);
	if(hash.hash) ggz_free(hash.hash);

	printf("Original: %s\n", orig);
	printf("Hashed:   %s\n", enc);
	printf("Expected: %s\n", expected);

	if(strcmp(enc, expected)) ret = 1;

	ggz_free(enc);

	return ret;
}
#endif

int main(int argc, char *argv[])
{
	int ret = 0;

	// md5, sha1, ripemd160

#ifdef USE_GCRYPT
	printf("md5 base16 encoding\n");
	printf("===================\n");
	ret |= test16conversion("hello", "md5", "5d41402abc4b2a76b9719d911017c592");
	ret |= test16conversion("abc", "md5", "900150983cd24fb0d6963f7d28e17f72");
	ret |= test16conversion("test", "md5", "098f6bcd4621d373cade4e832627b4f6");

	printf("\nmd5 base64 encoding\n");
	printf("===================\n");
	ret |= test64conversion("hello", "md5", "XUFAKrxLKna5cZ2REBfFkg==");
	ret |= test64conversion("abc", "md5", "kAFQmDzST7DWlj99KOF/cg==");
	ret |= test64conversion("test", "md5", "CY9rzUYh03PK3k6DJie09g==");

	printf("\nsha1 base16 encoding\n");
	printf("====================\n");
	ret |= test16conversion("hello", "sha1", "aaf4c61ddcc5e8a2dabede0f3b482cd9aea9434d");
	ret |= test16conversion("abc", "sha1", "a9993e364706816aba3e25717850c26c9cd0d89d");
	ret |= test16conversion("test", "sha1", "a94a8fe5ccb19ba61c4c0873d391e987982fbbd3");

	printf("\nsha1 base64 encoding\n");
	printf("====================\n");
	ret |= test64conversion("hello", "sha1", "qvTGHdzF6KLavt4PO0gs2a6pQ00=");
	ret |= test64conversion("abc", "sha1", "qZk+NkcGgWq6PiVxeFDCbJzQ2J0=");
	ret |= test64conversion("test", "sha1", "qUqP5cyxm6YcTAhz05Hph5gvu9M=");
#endif

	return ret;
}

