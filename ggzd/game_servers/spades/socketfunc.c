/*
 * File: socketfunc.c
 * Author: Brent Hendricks
 * Project: NetSpades
 * Date: 4/16/98
 *
 * My useful socket functions
 *
 * Copyright (C) 1998 Brent Hendricks.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */


#include <config.h>

#ifdef DEBUG_MEM
# include <dmalloc.h>
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <socketfunc.h>
#include <err_func.h>


int makesocket (unsigned short int port, short int type, char* server) {

  int sock;
  struct sockaddr_in name;
  struct hostent *hp;

  /* Create socket */
  if ( (sock = socket(PF_INET, SOCK_STREAM, 0)) < 0 ) {
    return( ERR_SOCK_OPEN );
  }
     
  name.sin_family = AF_INET;
  name.sin_port = htons (port);

  switch( type ) {

  case SERVER:
    name.sin_addr.s_addr = htonl (INADDR_ANY);
    if ( bind( sock, (struct sockaddr*)&name, sizeof(name)) < 0 ) {
      return( ERR_SOCK_BIND );
    }
    break;

  case CLIENT:
    if ( (hp = gethostbyname( server )) == NULL ) {
      return( ERR_HOST );
    }
    memcpy( &name.sin_addr, hp->h_addr, hp->h_length);
    if (connect(sock, (struct sockaddr*)&name, sizeof(name) ) == -1 ) {
      return( ERR_SOCK_CONN );
    }
    break;
  }

  return sock;

}


/*
 * Write a char string to the given fd
 */
int writestring( int msgsock, const char* message ) {

  int size = strlen(message)*sizeof(char) +1;
  int data = htonl( size );
  int status;
  
  status = write( msgsock, &data, sizeof(int) );
  
  if( status <= 0 ) {
    status = -1;
#ifdef DEBUG_SOCKET
    dbg_msg( "[%d]: Error sending string size\n" );
#endif
  }
  else {
#ifdef DEBUG_SOCKET
    dbg_msg( "[%d]: Sending \"%d\" : %d bytes \n", getpid(), size,
	    status );
#endif
    
    status = write( msgsock, message, size );

    if( status <= 0 ) {
      status = -1;
#ifdef DEBUG_SOCKET
      dbg_msg( "[%d]: Error sending string\n" );
#endif
    }
#ifdef DEBUG_SOCKET
    else {
      dbg_msg( "[%d]: Sending \"%s\" : %d bytes \n", getpid(), message,
	      status );
    }
#endif
  }

  return status;
  
}


/*
 * Read a char string from the given fd
 */
int readstring( int msgsock, char** message ) {

  int data, size, status;
  
  status = read( msgsock, &data, sizeof(int) );
  
  size = ntohl( data );


  if( status < 0 ) {
#ifdef DEBUG_SOCKET
    dbg_msg( "[%d]: Error receiving string size\n", getpid() );
#endif
  }
  else if( status == 0 ) {
#ifdef DEBUG_SOCKET
    dbg_msg( "[%d]: Warning: fd is closed\n", getpid() );
#endif
  }
  else {
#ifdef DEBUG_SOCKET
    dbg_msg( "[%d]: Received \"%d\" : %d bytes \n", getpid(), size,
	    status );
#endif
  
    *message = (char*)malloc( size*sizeof(char) );

    if( *message == NULL ) {
      dbg_msg( "[%d]: Error: Not enough memory\n", getpid() );
      status = -1;
    }
    else { 
      memset( *message, 0, size );
      status = read( msgsock, *message , size );

#ifdef DEBUG_SOCKET
      if( status < 0 ) {
	dbg_msg( "[%d]: Error receiving string\n", getpid() );
      }
      else if( status == 0 ) {
	dbg_msg( "[%d]: Warning: fd is closed\n", getpid() );
      }
      else {
	dbg_msg( "[%d]: Received \"%s\" : %d bytes \n", getpid(), *message,
		status );
      }
#endif

    } /* else not out of memory */ 
  } /* else status >0  */

  return status;
}


/*
 * Take a host-byte order int and write on fd using
 * network-byte order.
 */
int writeint( int msgsock, const int message ) {

  int status, data = htonl(message);

  status = write( msgsock, &data, sizeof(int) );

  if( status <= 0 ) {
    status = -1;
#ifdef DEBUG_SOCKET
    dbg_msg( "[%d]: Error sending int\n", getpid() );
#endif
  }
#ifdef DEBUG_SOCKET
  else {
    dbg_msg( "[%d]: Sending \"%d\" : %d bytes \n", getpid(), message, 
	    status );
  }
#endif
  
  return status;
}


/*
 * Read a network byte-order integer from the fd and
 * store it in host-byte order.
 */
int readint( int msgsock, int* message ) {

  int data, status;
  
  status = read(msgsock, &data, sizeof(int) );
  
  *message = ntohl( data );

#ifdef DEBUG_SOCKET
  if( status < 0 ) {
    dbg_msg( "[%d]: Error receiving int\n", getpid() );
  }
  else if( status == 0 ) {
    dbg_msg( "[%d]: Warning: fd is closed\n", getpid() );
  }
  else {
    dbg_msg( "[%d]: Received \"%d\" : %d bytes \n", getpid(), *message, 
	    status );
  }
#endif

  return status;
}
