#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
/*#include <sys/mman.h>*/
#include <sys/shm.h>
#include <sys/stat.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
/*#include <fcntl.h>*/
#include <errno.h>

#include <ggz.h>

static char buffer[1024], buffer2[1024];
static int shmid;

static void *reader(void *arg)
{
	int fd = *(int*)arg;
	int ret;
	char *shm;

	printf("Enable TLS for the server...\n");
	ret = ggz_tls_enable_fd(fd, GGZ_TLS_SERVER, GGZ_TLS_VERIFY_NONE);
	/*if(!ret) return NULL;*/
	printf("Read a message...\n");
	ggz_tls_read(fd, buffer2, sizeof(buffer2));

	shm = (char*)shmat(shmid, 0, 0);
	sprintf(shm, "%s", buffer2);
	shmdt(shm);

	return NULL;
}

static void *writer(void *arg)
{
	int fd = *(int*)arg;
	int ret;

	printf("Enable TLS for the client...\n");
	ret = ggz_tls_enable_fd(fd, GGZ_TLS_CLIENT, GGZ_TLS_VERIFY_NONE);
	/*if(!ret) return NULL;*/
	printf("Write a message...\n");
	ggz_tls_write(fd, buffer, strlen(buffer));

	return NULL;
}

static void timeout(int signal)
{
	exit(-1);
}

int main(int argc, char *argv[])
{
	int fd[2];
	/*pthread_t id_read, id_write;*/
	pid_t pid;
	char *shm;

	printf("Preparation...\n");
	/*ret = shm_open("Physical", O_RDWR, 0777);
	if(ret < 0)
	{
		fprintf(stderr, "SHM failed (errno = %i (%s))!\n", errno, strerror(errno));
		
		exit(-1);
	}*/
	shmid = shmget(IPC_PRIVATE, getpagesize(), IPC_CREAT | S_IRUSR | S_IWUSR);
	if(shmid < 0)
	{
		fprintf(stderr, "SHM failed (errno = %i (%s))!\n", errno, strerror(errno));
		exit(-1);
	}
	printf("SHM: size=%i\n", getpagesize());

	strcpy(buffer, "This is a test.");
	socketpair(AF_UNIX, SOCK_STREAM, 0, fd);

	/*pthread_create(&id_write, NULL, writer, &fd[0]);*/
	/*pthread_create(&id_read, NULL, reader, &fd[1]);*/

	signal(SIGALRM, timeout);
#ifdef HAVE_ALARM
	alarm(1);
#else
	/* Alarm isn't present on all platforms.  FIXME: implement without
	 * using it. */
	printf("This test cannot be completed on this platform.");
	timeout();
#endif

	pid = fork();
	switch(pid)
	{
		case -1:
			fprintf(stderr, "Fork failed!\n");
			exit(-1);
			break;
		case 0:
			reader((void*)&fd[1]);
			exit(0);
			break;
		default:
			/*sleep(1);*/
			writer((void*)&fd[0]);
			waitpid(pid, NULL, 0);
	}

	/*pthread_join(id_write, NULL);*/
	/*pthread_join(id_read, NULL);*/

	shm = (char*)shmat(shmid, 0, 0);
	printf("SHM: message=%s\n", shm);
	sprintf(buffer2, "%s", shm);
	shmdt(shm);

	printf("\n==========================\n");
	printf("Sent on [%i]: %s\n", fd[0], buffer);
	printf("Received on [%i]: %s\n", fd[1], buffer2);
	printf("Did we use encryption? %i\n", ggz_tls_support_query());

	if(strcmp(buffer, buffer2))
	{
		fprintf(stderr, "Encrypted transmission failed!\n");
		return -1;
	}

	return 0;
}

