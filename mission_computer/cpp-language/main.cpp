#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <syslog.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <time.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

//ssid: farm_land
//pw: farm_land
//cmake
//home/madman -> /home/ubuntu or ~/

static void daemonize(void);
void error_handling(char *message);

int main(int argc, char *argv[])
{
//	daemonize();
	int sock;
	char message[BUF_SIZE];
	
	UdpClient udp_client("192.168.6.1", 14550);
	sock = udp_client.getSocket();			

	if(sock == -1)
		error_handling((char*)"socket() error");
	
	bool ret = 0;
	ret = udp_client.requestHistory(5);
	
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

static void daemonize(void)
{
    pid_t pid, sid;

    /* already a daemon */
    if ( getppid() == 1 ) return;

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then we can exit the parent process. */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* At this point we are executing as the child process */

    /* Change the file mode mask */
    umask(0);

    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    /* Change the current working directory.  This prevents the current
       directory from being locked; hence not being able to remove it. */
    if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }

    /* Redirect standard files to /dev/null */
    freopen( "/dev/null", "r", stdin);
    freopen( "/dev/null", "w", stdout);
    freopen( "/dev/null", "w", stderr);
}