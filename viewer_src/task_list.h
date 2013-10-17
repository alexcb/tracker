#ifndef TRACKER_FILE_READER_H
#define TRACKER_FILE_READER_H

#include <vector>
#include <string>
#include <exception>

class NoTaskHistoryError : public std::exception
{
	virtual const char* what();
};

class Task
{
public:
	Task( const char *name, time_t last_logged_time ) : 
		name(name), 
		last_logged_time(last_logged_time)
	{}

	std::string name;
	time_t last_logged_time;
};


class LoggedTask
{
public:
	LoggedTask( Task *task, time_t time ) : 
		task( task ), 
		time( time )
	{}
	Task *task;
	time_t time;
};


class TaskSearchResult
{
public:
	TaskSearchResult(const char *name, int number_of_sub_tasks, time_t last_logged_time) :
		name(name),
		number_of_sub_tasks(number_of_sub_tasks),
		last_logged_time(last_logged_time)
	{}
	std::string name;
	int number_of_sub_tasks;
	time_t last_logged_time;
};


class TaskList
{
public:
	void load( const char *filename );
	void save( const char *filename = NULL );

	void addLoggedTask( const char *task_name, time_t task_start = 0 );

	//get details about all logged work
	unsigned int num_logged_tasks( Task *task ) const;
	inline unsigned int num_logged_tasks() const { return _logged_tasks.size(); }
	inline const LoggedTask* getLoggedTask( unsigned int i ) const { return &_logged_tasks[i]; }
	inline LoggedTask* getLoggedTask( unsigned int i ) { return &_logged_tasks[i]; }

	//get details about each task name (task names are unique)
	inline unsigned int num_task_names() const { return _tasks.size(); }
	inline const Task* getTask( unsigned int i ) const { return _tasks[i]; }
	inline Task* getTask( unsigned int i ) { return _tasks[i]; }
	Task* get_task( const char *task_name );
	void removeTask( Task *task );

	//given a task name, when it the last (starting) time it was logged
	time_t getLastTimeWorked( const char *name ) const;

	//return current task
	LoggedTask getCurrentTask() const;

	//return a list of task names matching a string
	std::vector<TaskSearchResult> findTasks( const char *search, const char *delim, time_t min_task_age ) const;
	
	void renameLoggedTask( LoggedTask *task, const char *new_name, bool rename_all_tasks = false );

protected:
	std::string _filename;
	std::vector< LoggedTask > _logged_tasks;
	std::vector< Task* > _tasks;

private:
	Task* _get_task_and_update_last_time( const char *task_name, time_t task_start_time );
};

//std::vector< Task > getTasks( const char* filename );
//void writeTasks( std::vector< Task > tasks, const char* filename );

std::string getTrackerTaskFile();
std::string getTrackerUserSettingsFile();

#endif //TRACKER_FILE_READER_H