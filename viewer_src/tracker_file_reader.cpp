#include "tracker_file_reader.h"
#include <iostream>
#include <sstream>

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

#define BUF_SIZE 2048

#ifdef WIN32
#include <Windows.h>
#endif //WIN32

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

// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
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

void writeTasks( std::vector< Task > tasks, const char* filename )
{
	std::string new_filename = std::string(filename) + ".new";
	FILE *fp = fopen(new_filename.c_str(), "w");
	if( fp == NULL )
		throw std::exception("couldnt open file - (TODO insert the error here)");
	for( std::vector<Task>::const_iterator i = tasks.begin(); i != tasks.end(); i++ ) {
		std::ostringstream oss;
		std::string s = i->name;
		oss << i->time << " " << trim( s ) << std::endl;
		std::string output_str = oss.str();
		fwrite( output_str.c_str(), output_str.length(), 1, fp );
	}
	fclose(fp);
	
	//TODO THIS IS NOT WORKING
#ifdef WIN32
	unlink( filename );
	BOOL ok = MoveFile( new_filename.c_str(), filename );
	if( !ok ) {
		DWORD error_code = GetLastError();
		error_code = error_code;
	}
#else
	assert(0); //TODO write this
#endif //WIN32
}