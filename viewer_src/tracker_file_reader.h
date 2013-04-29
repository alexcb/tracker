#ifndef TRACKER_FILE_READER_H
#define TRACKER_FILE_READER_H

#include <vector>
#include <string>

class Task
{
public:
	Task( const char *name, time_t time ) : name(name), time(time) {}
	std::string name;
	time_t time;
};

std::vector< Task > getTasks( const char* filename );
void writeTasks( std::vector< Task > tasks, const char* filename );

std::string getTrackerTaskFile();

#endif //TRACKER_FILE_READER_H