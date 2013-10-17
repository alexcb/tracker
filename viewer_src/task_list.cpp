#include "task_list.h"

#include "string_util.h"

#include <iostream>
#include <sstream>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <stdexcept>

#include <string.h>
#include <stdio.h>
#include <assert.h>

#define BUF_SIZE 2048

#ifdef WIN32

//Windows only includes
#include <Windows.h>

#else

//linux only includes
#include <pwd.h>
#include <unistd.h>

#endif //WIN32

const char* NoTaskHistoryError_str = "No recorded tasks";
const char* NoTaskHistoryError::what()
{
	return NoTaskHistoryError_str;
}



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

std::string getTrackerUserSettingsFile()
{
#ifdef WIN32
	return std::string("tracker_settings.txt");
#else
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;

	std::ostringstream oss;
	oss << homedir << "/.tracker_time_settings";
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



void writeTasks( std::vector< Task > tasks, const char* filename )
{

}


void TaskList::load( const char *filename )
{
	_filename = filename;
	_logged_tasks.clear();
	_tasks.clear();

	FILE *fp = fopen(filename, "r");
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
					addLoggedTask( p, time );
				}
			}
			if( feof(fp) )
				break;
		}
		fclose(fp);
	} else {
		//std::cerr << "error reading tasks" << std::endl;
	}
}

void TaskList::save( const char *filename )
{
	if( filename == NULL )
		filename = _filename.c_str();

	std::string new_filename = std::string(filename) + ".new";
	FILE *fp = fopen(new_filename.c_str(), "w");
	if( fp == NULL ) {
		std::cerr << "failed to open .new file" << std::endl;
		throw std::runtime_error("couldnt open file - (TODO insert the error here)");
	}
	for( std::vector<LoggedTask>::const_iterator i = _logged_tasks.begin(); i != _logged_tasks.end(); i++ ) {
		std::ostringstream oss;
		std::string s = i->task->name;
		oss << i->time << " " << trim( s ) << std::endl;
		std::string output_str = oss.str();
		fwrite( output_str.c_str(), output_str.length(), 1, fp );
	}
	fclose(fp);
	
#ifdef WIN32
	unlink( filename );
	BOOL ok = MoveFile( new_filename.c_str(), filename );
	if( !ok ) {
		DWORD error_code = GetLastError();
		error_code = error_code;
	}
#else
	//TODO check error code of this
	int result = rename( new_filename.c_str(), filename );
	if( result != 0 ) {
		std::cerr << "failed to write to disk " << result << std::endl;
		assert(0);
	}
#endif //WIN32
}


void TaskList::addLoggedTask( const char *task_name, time_t task_start )
{
	Task *task = _get_task_and_update_last_time( task_name, task_start );
	bool duplicate_task_entry = ( _logged_tasks.size() > 0 && _logged_tasks.back().task->name == task_name );

	if( !duplicate_task_entry ) {
		_logged_tasks.push_back( LoggedTask( task, task_start ) );
	}
}

//return true if lhs < rhs
bool compare_task(const Task* lhs, const Task* rhs)
{
	return lhs->name.compare( rhs->name ) < 0;
}

Task* TaskList::_get_task_and_update_last_time( const char *task_name, time_t task_start_time )
{
	Task *task = get_task( task_name );
	if( task ) {
		task->last_logged_time = task_start_time;
	} else {
		task = new Task( task_name, task_start_time );

		//ordered insert
		std::vector<Task*>::iterator itr = std::upper_bound(_tasks.begin(), _tasks.end(), task, compare_task);
		_tasks.insert(itr, task);
	}
	return task;
}

Task* TaskList::get_task( const char *task_name )
{
	Task fake_task( task_name, -1);
	std::vector<Task*>::iterator itr = std::lower_bound(_tasks.begin(), _tasks.end(), &fake_task, compare_task);
	
	if( itr != _tasks.end() && (*itr)->name == task_name )
		return *itr;

	return NULL;
}

void TaskList::removeTask( Task *task )
{
	std::vector<Task*>::iterator itr = std::lower_bound(_tasks.begin(), _tasks.end(), task, compare_task);
	
	if( itr != _tasks.end() && (*itr) == task ) {
		_tasks.erase( itr );
	}
}

time_t TaskList::getLastTimeWorked( const char *name ) const
{
	const Task *task = const_cast<TaskList*>(this)->get_task( name );
	return task ? task->last_logged_time : -1;
}

LoggedTask TaskList::getCurrentTask() const
{
	if( _logged_tasks.size() > 0 )
		return _logged_tasks.back();
	throw NoTaskHistoryError();
}

std::vector<TaskSearchResult> TaskList::findTasks( const char *search, const char *delim, time_t min_task_age ) const
{
	std::vector<TaskSearchResult> task_search_results;

	int number_of_sub_tasks_to_search = numberOfStringMatches(search, delim) + 1;

	for( int i = 0; i < _tasks.size(); i++ ) {
		if( _tasks[i]->last_logged_time > min_task_age ) {
			const char *task_name = _tasks[i]->name.c_str();
			const char *task_name_end = findNthString( task_name, delim, number_of_sub_tasks_to_search );
			int len_to_search = task_name_end ? task_name_end - task_name : strlen(task_name);
			const char *matched_s = strstr(task_name, search);

			if( matched_s && (matched_s - task_name) < len_to_search ) {
				std::string trimed_task(task_name, task_name + len_to_search);
				bool was_trimmed = trimed_task.length() != strlen(task_name);
				if( was_trimmed )
					trimed_task += delim;
				task_search_results.push_back( TaskSearchResult( trimed_task.c_str(), was_trimmed ? 1 : 0, _tasks[i]->last_logged_time ) );
			}
		}
	}

	//scan through all the results and group together matches
	std::vector<TaskSearchResult> grouped_task_search_results;
	for( int i = 0; i < task_search_results.size(); i++ ) {
		if( grouped_task_search_results.size() > 0 && \
			grouped_task_search_results.back().name == task_search_results[i].name && \
			grouped_task_search_results.back().number_of_sub_tasks > 0
			) {
			TaskSearchResult &task_search_result = grouped_task_search_results.back();
			task_search_result.number_of_sub_tasks += 1;
			if( task_search_results[i].last_logged_time > task_search_result.last_logged_time )
				task_search_result.last_logged_time = task_search_results[i].last_logged_time;
		} else {
			grouped_task_search_results.push_back( task_search_results[i] );
		}
	}

	return grouped_task_search_results;
}

void TaskList::renameLoggedTask( LoggedTask *task, const char *new_name, bool rename_all_tasks )
{
	Task *old_task = task->task;
	Task *new_task = _get_task_and_update_last_time( new_name, old_task->last_logged_time );

	if( rename_all_tasks ) {
		for( int i = 0; i < _logged_tasks.size(); i++ ) {
			if( _logged_tasks[i].task == old_task )
				_logged_tasks[i].task = new_task;
		}
	} else {
		task->task = new_task;
	}

	bool remove_old_task = true;
	for( int i = 0; i < _logged_tasks.size(); i++ ) {
		if( _logged_tasks[i].task == old_task )
			remove_old_task = false;
	}
	if( remove_old_task == true ) {
		removeTask( old_task );
	}
}

unsigned int TaskList::num_logged_tasks( Task *task ) const
{
	unsigned int found_logged_tasks = 0;
	for( unsigned int i = 0; i < _logged_tasks.size(); i++ ) {
		if( _logged_tasks[i].task == task )
			found_logged_tasks++;
	}
	return found_logged_tasks;
}