#ifndef TRACKER_WRITER_H_SFSDF
#define TRACKER_WRITER_H_SFSDF

#include <set>
#include <string>
#include <time.h>

time_t logTask(const char *task, time_t task_started_time = 0);

//return a list of all the tasks in the 
std::set< std::string > getPreviousTasks();

//return the last logged task, or ""
std::string getLastTask( time_t &task_started_at );

#endif //TRACKER_WRITER_H_SFSDF
