#ifndef WIN32

#include "tracker_ipc.h"

#include <stdio.h>
#include <iostream>
#include <pwd.h>
#include <string>
#include <sstream>
#include <signal.h>
#include <unistd.h>

#define BUFF_SIZE 1024

std::string getPIDFile()
{
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;

	std::ostringstream oss;
	oss << homedir << "/.tracker_pid";
	return oss.str();
}

pid_t readPID(const char *fname)
{
	pid_t pid = 0;
	FILE *fp = fopen(fname, "r");
	if( fp ) {
		if( fscanf(fp, "%d\n", &pid) != 1 ) {
			pid = 0;
		}
		fclose(fp);
	}
	return pid;
}

void writePID()
{
	const char *fname = getPIDFile().c_str();
	pid_t pid = getpid();
	FILE *fp = fopen(fname, "w");
	if( fp ) {
		fprintf(fp, "%d\n", pid);
		fclose(fp);
	} else {
		std::cerr << "error writing pid" << std::endl;
	}
}

bool signalOtherInstance()
{
	const char *fname = getPIDFile().c_str();
	pid_t pid = readPID( fname );
	std::cout << "read pid: " << pid << std::endl;
	return ( pid > 0 && kill(pid, SIGUSR1) == 0 );
}

#endif //win32
