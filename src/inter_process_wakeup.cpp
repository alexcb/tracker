#include "inter_process_wakeup.h"

#include <iostream>
#include <pwd.h>
#include <string>
#include <sstream>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

void (*_wakeup_handler)() = NULL;

void signal_handler( int signo )
{
	if( _wakeup_handler )
		_wakeup_handler();
}

//Returns True when it works, False otherwise
bool signalOtherInstance(pid_t pid)
{
    return ( pid > 0 && kill(pid, SIGUSR1) == 0 );
}

void PassPtr(void (*wakeup_handler)())
{
}

// Register a callback function (to be called when another process singals)
bool setupWakeupHandler(void (*wakeup_handler)())
{
	_wakeup_handler = wakeup_handler;
	return ( signal( SIGUSR1, signal_handler) != SIG_ERR );
}

// Returns the full path to the PID file
std::string getPIDFilePath()
{
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;

    std::ostringstream oss;
    oss << homedir << "/.tracker_pid";
    return oss.str();
}

// Reads the PID stored in the file
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

void writePID(const char *fname)
{   
    pid_t pid = getpid();
    FILE *fp = fopen(fname, "w");
    if( fp ) {
        fprintf(fp, "%d\n", pid);
        fclose(fp);
    } else {
        std::cerr << "error writing pid" << std::endl;
    }
}   

