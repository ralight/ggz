#include <stdio.h>
#include <ggzcore.h>
#include <stdlib.h>
#include <string.h>

// URI:
// query://connection/0.0.5pre
// Answer:
// ggz://jzaun.com:5688

#define METASERVDIR "/home/josef"

char *metaserv_lookup(const char *category, const char *key)
{
	static char *ret = NULL;
	int handler;

	if(ret)
	{
		free(ret);
		ret = NULL;
	}
	handler = ggzcore_confio_parse(METASERVDIR "/.ggz/metaserv.rc", GGZ_CONFIO_RDONLY);
	if(handler < 0) return NULL;
	ret = ggzcore_confio_read_string(handler, category, key, NULL);
	return ret;
}

char *metamagic(char *uri)
{
	static char *ret = NULL;
	char *token;
	char *category;

	if(!uri) return NULL;
	if(ret)
	{
		free(ret);
		ret = NULL;
	}
	if(uri[strlen(uri) - 1] == '\n') uri[strlen(uri) - 1] = 0;

	// query://connection/0.0.5pre
	token = strtok(uri, ":/");
	// query connection 0.0.5pre
	if(!token) return NULL;
	if(!strcmp(token, "query"))
	{
		token = strtok(NULL, ":/");
		// connection 0.0.5pre
		if(!token) return NULL;
		category = strdup(token);
		token = strtok(NULL, ":/");
		// 0.0.5pre
		if(token) ret = metaserv_lookup(category, token);
		free(category);
	}
	while(token) token = strtok(NULL, ":/");

	return ret;
}

int main(int argc, char *argv[])
{
	char buffer[1024];
	char *result;

	while(1)
	{
		fgets(buffer, sizeof(buffer), stdin);
		result = metamagic(strdup(buffer));
		printf("Got URI: %s\n", result);
	}
}


