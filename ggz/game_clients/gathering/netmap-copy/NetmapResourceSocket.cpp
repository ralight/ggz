////////////////////////////////////////////////////////////////////////////
//                                                                        //
// The Netmap C++ Class Library                                           //
//                                                                        //
// Copyright (C) 2001 Josef Spillner                                      //
// dr_maux@users.sourceforge.net                                          //
// The MindX Open Source Project                                          //
// http://mindx.sourceforge.net/development/netmap                        //
//                                                                        //
// Published under GNU GPL conditions                                     //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

// Netmap include files
#include <NetmapResourceSocket>

// System include files
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int m_socket;

/** Constructor */
NetmapResourceSocket::NetmapResourceSocket()
{
}

/** Destructor */
NetmapResourceSocket::~NetmapResourceSocket()
{
}

/** Open a socket locally. This is for reading and writing. */
void NetmapResourceSocket::openSocket(char *socketname)
{
	char* fd_name;
	struct sockaddr_un addr;

	if((m_socket = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0) return;

	if((fd_name = (char*)malloc(strlen(socketname) + 32)) == NULL) return;
	sprintf(fd_name, "/tmp/%s.%d", socketname, getpid());

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_LOCAL;
	strcpy(addr.sun_path, fd_name);
	free(fd_name);

	if(::connect(m_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) return;
}

/** Close an open socket. */
void NetmapResourceSocket::closeSocket()
{
}

/** Specify the x and y coordinates to read one tile. */
Ntile *NetmapResourceSocket::readTileAbsolute(Ncoord x, Ncoord y)
{
}

/** Read a tile relative to the specified Vtile. */
Ntile *NetmapResourceSocket::readTileRelative(Ncoord vx, Ncoord vy, Ncoord x, Ncoord y)
{
}

/** Specify the vx and vy coordinates to read one vtile. */
Nvtile *NetmapResourceSocket::readVtileAbsolute(Ncoord vx, Ncoord vy)
{
}

