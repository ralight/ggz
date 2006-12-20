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

#ifndef GGZCONF
#error GGZ configuration directory is not known!
#endif

/* Standard trigger declarations */
extern Datum ggzsync(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(ggzsync);
#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

/* Global variables */
PGconn *peerconn = NULL;

/* Helper function to initialise the peer connection */
static void ggzsync_init(void)
{
	char configfile[256];
	char conninfo[256];
	const char *dbhost, *dbname, *dbuser, *dbpass;
	int rc;

	snprintf(configfile, sizeof(configfile), GGZCONF "/ggz2phpbb.conf");
	rc = ggz_conf_parse(configfile, GGZ_CONF_RDONLY);
	if(rc == -1)
		elog(ERROR, "ggzsync_init: Could not open configuration file at '%s'",
			configfile);

	dbhost = ggz_conf_read_string(rc, "Forum", "DatabaseHost", NULL);
	dbname = ggz_conf_read_string(rc, "Forum", "DatabaseName", NULL);
	dbuser = ggz_conf_read_string(rc, "Forum", "DatabaseUsername", NULL);
	dbpass = ggz_conf_read_string(rc, "Forum", "DatabasePassword", NULL);
	ggz_conf_close(rc);

	snprintf(conninfo, sizeof(conninfo),
		"host=%s dbname=%s user=%s password=%s",
		dbhost, dbname, dbuser, dbpass);

	peerconn = PQconnectdb(conninfo);
	if((!peerconn) || (PQstatus(peerconn) == CONNECTION_BAD))
		elog(ERROR, "ggzsync_init: Could not connect to peer database.");
}

/* Helper function to write out SQL null values */
static void ggzsync_null(char *buf, int buflen, const char *value)
{
	if(value == NULL)
		snprintf(buf, buflen, "NULL");
	else
		snprintf(buf, buflen, "'%s'", value);
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
	const char *value;
	PGresult *res;
	char query[2048];
	const char *c_handle, *c_password, *c_email, *c_country;
	const char *c_id, *c_stamp, *c_stamp_default;
	char c_email_buf[128], c_country_buf[128];

	/* on the first run, do a lazy-connection */
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
	/* FIXME: safety checks go here (number == 8, rettupley<->trigtuple etc.) */
	tupdesc = trigdata->tg_relation->rd_att;
	columns = tupdesc->natts;
	elog(INFO, "[debug] Columns %i", columns);

	for(i = 0; i < columns; i++)
	{
		Form_pg_attribute a = tupdesc->attrs[i];
		NameData name = a->attname;

		value = SPI_getvalue(rettuple, tupdesc, i + 1);

		elog(INFO, "[debug] - column %i=%s: %s", i, name.data, value);
	}

	/* fetch the entire tuple as strings */
	c_id = SPI_getvalue(rettuple, tupdesc, 1);
	c_handle = SPI_getvalue(rettuple, tupdesc, 2);
	c_password = SPI_getvalue(rettuple, tupdesc, 3); /* FIXME: ensure md5! */
	c_email = SPI_getvalue(rettuple, tupdesc, 5);
	c_country = NULL; /* FIXME: this is only in userinfo */
	c_stamp = SPI_getvalue(rettuple, tupdesc, 6);

	ggzsync_null(c_email_buf, sizeof(c_email_buf), c_email);
	ggzsync_null(c_country_buf, sizeof(c_country_buf), c_country);

	c_stamp_default = (c_stamp ? c_stamp : "0");

	/* synchronise the tuple */
	if(TRIGGER_FIRED_BY_INSERT(trigdata->tg_event))
	{
		elog(INFO, "[debug] insert!");

		snprintf(query, sizeof(query),
			"INSERT INTO phpbb_users "
			"(user_id, username, user_password, user_email, user_from, user_active, user_level, user_regdate) VALUES "
			"(%s, '%s', '%s', %s, %s, 1, 0, %s)",
			c_id, c_handle, c_password, c_email_buf, c_country_buf, c_stamp_default);
		res = PQexec(peerconn, query);

		if(PQresultStatus(res) != PGRES_COMMAND_OK)
		{
			elog(INFO, "[debug] tried query: %s", query);
			elog(ERROR, "ggzsync: Sync insert failed.");
		}

		PQclear(res);
	}
	else if(TRIGGER_FIRED_BY_DELETE(trigdata->tg_event))
	{
		elog(INFO, "[debug] delete!");
		/* FIXME: we don't even know yet if GGZ accounts will be deleted and how so */
	}
	else if(TRIGGER_FIRED_BY_UPDATE(trigdata->tg_event))
	{
		elog(INFO, "[debug] update!");

		snprintf(query, sizeof(query),
			"UPDATE phpbb_users "
			"SET user_password = '%s' "
			"WHERE user_id = %s",
			c_password, c_id);
		res = PQexec(peerconn, query);
		/* FIXME: we should probably sync other things as well */

		if(PQresultStatus(res) != PGRES_COMMAND_OK)
		{
			elog(INFO, "[debug] tried query: %s", query);
			elog(ERROR, "ggzsync: Sync update failed.");
		}

		PQclear(res);
	}

	return PointerGetDatum(rettuple);
}

