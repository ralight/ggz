#include "shadownet.h"

#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

#include <qsocketnotifier.h>
#include <kmessagebox.h>

ShadowNet::ShadowNet()
{
}

ShadowNet::~ShadowNet()
{
}

void ShadowNet::addClient(const char *name, const char *commandline)
{
	char *tmp;
	struct stat st;
	char *const options[] = {(char*)commandline, NULL};

	tmp = strdup(commandline);
	list.append(tmp);

	cout << "CREATE FAKE SOCKET for " << name << endl;

	int sock;
	struct sockaddr_un sa;
	QSocketNotifier *snr, *snw;
	char *sockfile;
	int ret;
	int acc;
	int fd;

	ret = fork();
	sockfile = (char*)malloc(128);
	sprintf(sockfile, "/tmp/KTicTacTux.%d", ret);
	switch(ret)
	{
		case -1:
			return;
			break;
		case 0:
			while (stat(sockfile, &st) < 0) sleep(1);
			execvp(commandline, options);
			exit(-1);
			break;
		default:
			break;
	}

	sock = socket(PF_LOCAL, SOCK_STREAM, 0);
	memset(&sa, 0, sizeof(sa));
	sa.sun_family = AF_LOCAL;
	strncpy(sa.sun_path, sockfile, sizeof(sa.sun_path) - 1);
	unlink(sockfile);
	acc = bind(sock, (struct sockaddr*)&sa, SUN_LEN(&sa));
	if(listen(acc, 1) < 0) return;
	fd = accept(acc, NULL, NULL);
	if(fd < 0) return;

	snr = new QSocketNotifier(sock, QSocketNotifier::Read, this);
	connect(snr, SIGNAL(activated(int)), SLOT(slotRead(int)));
	snw = new QSocketNotifier(sock, QSocketNotifier::Write, this);
	connect(snw, SIGNAL(activated(int)), SLOT(slotWrite(int)));
}

void ShadowNet::start()
{
	char fd_name[64];
	struct sockaddr_un addr;
	int error;
	int fd;

	error = 0; 
	sprintf(fd_name, "/tmp/ShadowBridge.%d", getpid());
	if((fd = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0) error = 1;
	else
	{
		bzero(&addr, sizeof(addr));
		addr.sun_family = AF_LOCAL;
		strcpy(addr.sun_path, fd_name);
		if(::connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) error = 1;
	}
	if(error) KMessageBox::error(NULL, "Couldn't connect to ggzcore!", "Connection error");
}

void ShadowNet::slotRead(int sock)
{
	cout << "Read from socket: " << sock << endl;
}

void ShadowNet::slotWrite(int sock)
{
	cout << "Write to socket: " << sock << endl;
}

