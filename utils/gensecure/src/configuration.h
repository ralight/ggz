/**************************************************************************
GenSecure - a generic client/server example of TLS support for a connection
(C) 2001 Josef Spillner, dr_maux@users.sourceforge.net
Published under GNU GPL conditions
***************************************************************************/

/* configuration: Default configuration for the gensecure example. */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

/* Client connects to this host */
#define GENSECURE_HOST "localhost"

/* Port to be used be client and server */
#define GENSECURE_PORT 9998

/* Location of the server PEM certificate */
//#define GENSECURE_CERTIFICATE "/usr/local/ssl/certs/server.cert"
#define GENSECURE_CERTIFICATE "/usr/local/ssl/certs/openssl-example.pem"

/* Location of the server private key */
//#define GENSECURE_KEY "/usr/local/ssl/certs/server.key"
#define GENSECURE_KEY "/usr/local/ssl/certs/openssl-example.pem"

/* Password used to decrypt private key */
#define GENSECURE_PASSWORD "ggz rocks more"

#endif

