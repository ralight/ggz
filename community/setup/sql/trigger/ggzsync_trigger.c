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
PG_FUNCTION_INFO_V1(ggzsync);
#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

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
	int i;

	int columns;
	char *value;
	PGresult *res;
	char query[2048];
	char *c_handle, *c_password;

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

	/* find out the values which are in the active tuple */
	tupdesc = trigdata->tg_relation->rd_att;
	columns = tupdesc->natts;
	elog(INFO, "[debug] Columns %i", columns);

	for(i = 0; i < columns; i++)
	{
		Form_pg_attribute a = tupdesc->attrs[i];
		NameData name = a->attname;

		value = SPI_getvalue(trigdata->tg_trigtuple, tupdesc, i + 1);

		elog(INFO, "[debug] - column %i=%s: %s", i, name.data, value);
	}

	c_handle = SPI_getvalue(trigdata->tg_trigtuple, tupdesc, 1);
	c_password = SPI_getvalue(trigdata->tg_trigtuple, tupdesc, 2);

	/* Synchronise the tuple */
	if(TRIGGER_FIRED_BY_INSERT(trigdata->tg_event))
	{
		elog(INFO, "[debug] insert!");

		snprintf(query, sizeof(query),
			"INSERT INTO users2 (username, password) VALUES ('%s', '%s')",
			c_handle, c_password);
		res = PQexec(peerconn, query);

		if(PQresultStatus(res) != PGRES_COMMAND_OK)
		{
			elog(ERROR, "ggzsync: Sync insert failed.");
		}

		PQclear(res);
	}

	return PointerGetDatum(rettuple);
}

