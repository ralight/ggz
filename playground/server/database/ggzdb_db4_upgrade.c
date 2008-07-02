#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <config.h>

#include "datatypes.h"
#include "ggzdb.h"
#include "parse_conf.h"

Options opt;

/* Version 0.7 */
typedef struct {
	unsigned int user_id;           /* Numeric UID */
	char handle[MAX_USER_NAME_LEN+1];   /* Players nickname/handle */
	char password[41];          /* Players password (probably hashed) */
	char name[33];              /* Players real name */
	char email[33];             /* Players email address */
	time_t last_login;          /* Last login */
	unsigned int perms;         /* Permission settings */
	unsigned int confirmed;         /* Confirmation status */
} ggzdbPlayerEntry_0_7;

int find_ggzd_process(void)
{
	const char pid_filename[] = "ggzd.pid";
	char pid_file[strlen(opt.data_dir) + strlen(pid_filename) + 2];
	FILE *pidptr;

	snprintf(pid_file, sizeof(pid_file), "%s/%s", opt.data_dir, pid_filename);

	pidptr = fopen(pid_file, "r");
	if(pidptr){
		fclose(pidptr);
		return 1;
	}else{
		return 0;
	}
}

void get_current_version(char *vid, int len)
{
	const char *suffix = "/ggzdb.ver";
	char fname[strlen(opt.db.datadir) + strlen(suffix) + 1];
	FILE *vfile;
	char *newline;

	snprintf(fname, sizeof(fname), "%s%s", opt.db.datadir, suffix);

	if((vfile = fopen(fname, "r")) == NULL){
		printf("Database version file not found, aborting upgrade.\n"
				"Database version file location: %s\n", fname);
		exit(1);
	}else{
		fgets(vid, len, vfile);
		fclose(vfile);
	}

	newline = strchr(vid, '\n');
	if(newline){
		newline[0] = '\0';
	}
}


void write_current_version(void)
{
	const char *suffix = "/ggzdb.ver";
	char fname[strlen(opt.db.datadir) + strlen(suffix) + 1];
	FILE *vfile;

	snprintf(fname, sizeof(fname), "%s%s", opt.db.datadir, suffix);

	if((vfile = fopen(fname, "w")) == NULL){
		printf("Error: Unable to update ggzdb.ver.\n");
		exit(1);
	}else{
		fprintf(vfile, "%s", GGZDB_VERSION_ID);
		fclose(vfile);
	}
}


int move_database(const char *name, const char *vid)
{
	char oldpath[strlen(opt.db.datadir) + strlen(name) + 1];
	char newpath[strlen(opt.db.datadir) + strlen(name) + strlen(vid) + 2];
	
	snprintf(oldpath, sizeof(oldpath), "%s%s", opt.db.datadir, name);
	snprintf(newpath, sizeof(newpath), "%s%s_%s", opt.db.datadir, name, vid);

	return rename(oldpath, newpath);
}


int main(int argc, char *argv[])
{
	char vid[7];    /* Space for 123.45 */

	parse_conf_file();

	if(find_ggzd_process()){
		printf("ggzd appears to be running. Ensure it is not running before attempting a database upgrade.\n");
		return 1;
	}

	get_current_version(vid, 7);
	
	if(!strncmp(GGZDB_VERSION_ID, vid, strlen(GGZDB_VERSION_ID))){
		printf("Database already at latest version (%s)\n", GGZDB_VERSION_ID);
		return 0;
	}

	if(move_database("player.db", vid) < 0){
		fprintf(stderr, "Error moving player.db: %s\n", strerror(errno));
		return 1;
	}

	if(move_database("stats.db", vid) < 0){
		fprintf(stderr, "Error moving stats.db: %s\n", strerror(errno));
		return 1;
	}

	/* FIXME - init new database, open both old and new databases and copy
	 * across old data. */

	write_current_version();

	return 0;
}

