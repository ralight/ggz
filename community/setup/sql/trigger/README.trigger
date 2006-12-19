The 'ggzsync' trigger for the GGZ database
==========================================

The purpose of 'ggzsync' is to keep the GGZ user database in sync with other
databases, such as the user database of the phpBB forum software.
This is one of three possible ways to achieve such a synchrony:
* letting both use the same table
* using a script (cron job) such as 'ggz2phpbb.pl'
* using 'ggzsync'

The advantage of 'ggzsync' over the first approach is that it works with
unmodified table structures, so it can work with any forum software without
having to constantly modify the SQL schema for GGZ!
The advantage over the second approach is that it only gets activated when
really needed, and thus creates little overhead and full synchrony.

The disadvantage is that it only works with PostgreSQL. However, this is the
recommended database for larger installations.

For compilation, PostgreSQL 8.2 is recommended, although any 8.x should work.
The 'pg_config' utility must be installed.

Call 'make', followed by 'make install', to copy the compiled trigger into
the database server.

Afterwards, the two following functions must be called within the GGZ
database:

CREATE FUNCTION ggzsync() RETURNS trigger AS '$libdir/ggzsync_trigger.so' LANGUAGE C;
CREATE TRIGGER tggzsync BEFORE INSERT OR UPDATE OR DELETE ON users FOR EACH ROW EXECUTE PROCEDURE ggzsync();

