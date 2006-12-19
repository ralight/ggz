/*
 * Trigger for the synchronisation between GGZ user database and a forum
 * user database (e.g. phpBB) for use with PostgreSQL databases.
 * The origin of this code is the 'complete example' for a trigger in C
 * from the PostgreSQL reference documentation.
 *
 * This code runs as a plugin directly in the database server. As such
 * it is subject to careful changes only!
 */

/* Server-side includes */
#include <postgres.h>
#include <executor/spi.h>
#include <commands/trigger.h>

/* Client-side includes */
#include <../libpq-fe.h>
#include <ggz.h>

/* Standard trigger declarations */
extern Datum ggzsync(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(trigf);
PG_MODULE_MAGIC;

/* Global variables */
PGconn *peerconn = NULL;

/* Helper function to initialise the peer connection */
static void ggzsync_init()
{
	char configfile[256];
	char conninfo[256];
	const char *dbhost, *dbname, *dbuser, *dbpass;
	int rc;

	snprintf(configfile, sizeof(configfile), "/tmp/peers.conf");
	rc = ggz_conf_parse(configfile, GGZ_CONF_RDONLY);
	if(rc == -1)
		elog(ERROR, "ggzsync_init: Could not open configuration file at '%s'",
			configfile);

	dbhost = ggz_conf_read_string(rc, "Forum", "ForumHost", NULL);
	dbname = ggz_conf_read_string(rc, "Forum", "ForumName", NULL);
	dbuser = ggz_conf_read_string(rc, "Forum", "ForumUsername", NULL);
	dbpass = ggz_conf_read_string(rc, "Forum", "ForumPassword", NULL);
	ggz_conf_close(rc);

	snprintf(conninfo, sizeof(conninfo),
		"host=%s dbname=%s user=%s password=%s",
		dbhost, dbname, dbuser, dbpass);

	peerconn = PQconnectdb(conninfo);
	if((!peerconn) || (PQstatus(peerconn) == CONNECTION_BAD))
		elog(ERROR, "ggzsync_init: Could not connect to peer database.");
}

/* The main trigger function to synchronise the user databases */
Datum ggzsync(PG_FUNCTION_ARGS)
{
	TriggerData *trigdata = (TriggerData*)fcinfo->context;
	TupleDesc tupdesc;
	HeapTuple rettuple;
	bool checknull = false;
	bool isnull;
	int ret, i;

	/* On the first run, do a lazy-connection */
	if(!peerconn)
		ggzsync_init();

	/* make sure it's called as a trigger at all */
	if(!CALLED_AS_TRIGGER(fcinfo))
		elog(ERROR, "ggzsync: Not called by trigger manager.");

	/* tuple to return to executor */
	if(TRIGGER_FIRED_BY_UPDATE(trigdata->tg_event))
		rettuple = trigdata->tg_newtuple;
	else
		rettuple = trigdata->tg_trigtuple;

	/* check for null values */
	if(!TRIGGER_FIRED_BY_DELETE(trigdata->tg_event)
	&& TRIGGER_FIRED_BEFORE(trigdata->tg_event))
		checknull = true;

	tupdesc = trigdata->tg_relation->rd_att;

	/* connect to SPI manager */
	if((ret = SPI_connect()) < 0)
		elog(INFO, "ggzsync: SPI connection returned failed.");

	/* get number of rows in table */
	ret = SPI_exec("SELECT count(*) FROM users", 0);

	if(ret < 0)
		elog(NOTICE, "ggzsync: SPI query failed.");

	/* count(*) returns int8, so be careful to convert */
	i = DatumGetInt64(SPI_getbinval(SPI_tuptable->vals[0],
		SPI_tuptable->tupdesc, 1, &isnull));

	elog(INFO, "ggzsync: There are %d rows in users.", i);

	SPI_finish();

	if (checknull)
	{
		SPI_getbinval(rettuple, tupdesc, 1, &isnull);
		if (isnull)
			rettuple = NULL;
	}

	return PointerGetDatum(rettuple);
}

