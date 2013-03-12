#include "tracker_writer.h"

#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <vector>
#include <string.h>

#ifndef WIN32
#include <pwd.h>
#include <unistd.h>
#endif //WIN32

#define BUF_SIZE 2048

char* trim(char *s)
{
	while( *s == ' ' || *s == '\t' || *s == '\n' || *s == '\r' )
		s++;
	if( strlen(s) > 0 ) {
		char *end = s + strlen(s) - 1;
		while( *end == '\n' || *end == '\r' || *end == ' ' || *end == '\t' )
			*end = '\0';
	}
	return s;
}

std::string getTrackerTaskFile()
{
#ifdef WIN32
	return std::string("tracker_time.txt");
#else
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;

	std::ostringstream oss;
	oss << homedir << "/.tracker_time";
	return oss.str();
#endif //WIN32
}

time_t logTask(const char *task, time_t task_started_time)
{
	time_t task_started_at;
	if( getLastTask(task_started_at).compare( task ) == 0 )
		//dont write the task if it's the same one as the last logged task
		return task_started_at;

	if( task_started_time == 0 )
		task_started_time = time(NULL);
		
	std::string tmp = getTrackerTaskFile();
	const char *fname = tmp.c_str();

	FILE *fp = fopen(fname, "a");
	if( fp ) {
		fprintf(fp, "%lld %s\n", (long long) task_started_time, task);
		fclose(fp);
	} else {
		std::cerr << "error writing task" << std::endl;
	}
	return task_started_time;
}

std::string getLastTask( time_t &task_started_at )
{
	std::string last_task;
	std::string tmp = getTrackerTaskFile();
	const char *fname = tmp.c_str();
	FILE *fp = fopen(fname, "r");
	char buf[ BUF_SIZE ];
	if( fp ) {
		while(true) {
			char *s = fgets( buf, BUF_SIZE, fp );
			if( s ) {
				unsigned long int foo;
				sscanf( s, "%lu", &foo );
				task_started_at = foo;
				s = strstr(s, " ");
				if( s ) {
					last_task = trim(s);
					}
				}
			if( feof(fp) )
				break;
		}
		fclose(fp);
	}
	return last_task;
}

std::set< std::string > getPreviousTasks()
{
	std::set< std::string > tasks;
	std::string tmp = getTrackerTaskFile();
	const char *fname = tmp.c_str();
	FILE *fp = fopen(fname, "r");
	char buf[ BUF_SIZE ];
	if( fp ) {
		while(true) {
			char *s = fgets( buf, BUF_SIZE, fp );
			if( s ) {
				s = strstr(s, " ");
				if( s ) {
					s = trim(s);
					tasks.insert( std::string( s ) );
				}
			}
			if( feof(fp) )
				break;
		}
		fclose(fp);
	} else {
		std::cerr << "error reading tasks" << std::endl;
	}
	return tasks;
}

