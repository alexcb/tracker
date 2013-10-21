#include "task_list.h"

#include "QtCore\qabstractitemmodel.h"

#include <set>

class TaskList;
class UserSettings;

class TrackedTasksListModel : public QAbstractListModel
{
public:
    explicit TrackedTasksListModel( UserSettings *user_settings, TaskList *task_list, QObject *parent = 0 );

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role) const;

	void setSearchTerm(const QString search_term);


	//New methods in this model

	//returns the number of seconds spent on this task today
	time_t seconds_today(const QModelIndex &index) const;

	//returns the number of seconds since any time was logged to this task
	time_t seconds_last_active(const QModelIndex &index) const;

	//returns the number of sub tasks
	int number_of_sub_tasks(const QModelIndex &index) const;
	int number_of_sub_tasks(int row) const;

private:
	//results which are returned to user
	std::vector< TaskSearchResult > _task_search_results;

	const TaskSearchResult* getTaskSearchResultByName( const std::string& name ) const;

	UserSettings *_user_settings;
	TaskList *_task_list;
};