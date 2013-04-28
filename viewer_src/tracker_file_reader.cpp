#include "tracker_file_reader.h"
#include <iostream>
#include <sstream>

#define BUF_SIZE 2048

//TODO copied out of tracker
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

//another bullshit function
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

std::vector< Task > getTasks( const char* filename )
{
	std::vector< Task > tasks;
	std::string tmp = getTrackerTaskFile();
	const char *fname = tmp.c_str();
	FILE *fp = fopen(fname, "r");
	char buf[ BUF_SIZE ];
	if( fp ) {
		while(true) {
			char *s = fgets( buf, BUF_SIZE, fp );
			char *p;
			if( s ) {
				p = strstr(s, " ");
				if( p ) {
					p = trim(p);
					time_t time = atol(s);
					tasks.push_back( Task(p, time ) );
				}
			}
			if( feof(fp) )
				break;
		}
		fclose(fp);
	} else {
		//std::cerr << "error reading tasks" << std::endl;
	}
	return tasks;
}