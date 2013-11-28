#include "tracked_tasks_list_model.h"

#include "user_settings.h"

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <qcombobox.h>
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QTreeView>
#include <QListView>
#include <QPainter>
#include <QItemDelegate>

#include <qapplication.h>

#include <sstream>

#include <QCompleter>

#include <QFileSystemModel>

#include <assert.h>

#include <QKeyEvent>

#include "task_list.h"

#include <time.h>


#ifdef WIN32
#include <Windows.h>
#endif //WIN32

TrackedTasksListModel::TrackedTasksListModel( UserSettings *user_settings, TaskList *task_list, QObject *parent ) : 
	QAbstractListModel( parent ),
	_user_settings( user_settings ),
	_task_list( task_list )
{}

int num_occurances_in_string( const char *string, const char *term )
{
	int num_found = 0;
	while( string ) {
		string = strstr(string, term);
		if( string ) {
			string++;
			num_found++;
		}
	}
	return num_found;
}

std::string substring_up_to_delim( const char *string, const char *delim, int num_delims )
{
	const char *end = string;
	for( ; num_delims > 0 && end; num_delims--  ) {
		end = strstr(end, delim);
		if( end )
			end++;
	}

	if( end )
		return std::string(string, end);
	return std::string(string);
}

void TrackedTasksListModel::setSearchTerm( const QString search_term )
{
	time_t min_task_age = time(NULL) - _user_settings->auto_complete_show_tasks_newer_than_days * 60*60*24;

	const char *delim = " - ";

	beginResetModel();

	_task_search_results = _task_list->findTasks( search_term.toUtf8().constData(), delim, min_task_age );
	
	endResetModel();
}

int TrackedTasksListModel::rowCount( const QModelIndex &parent ) const
{
	return _task_search_results.size();
}

QVariant TrackedTasksListModel::data( const QModelIndex &index, int role ) const
{
    if( index.row() < 0 || index.row() >= _task_search_results.size() )
        return QVariant();

    if( role == Qt::DisplayRole || role == Qt::EditRole )
        return QString::fromUtf8( _task_search_results[index.row()].name.c_str() );

    return QVariant();
}

int TrackedTasksListModel::number_of_sub_tasks( int row ) const
{
    if( row < 0 || row >= _task_search_results.size() )
        return -1;

	return _task_search_results[ row ].number_of_sub_tasks;
}

int TrackedTasksListModel::number_of_sub_tasks( const QModelIndex &index ) const
{
	return number_of_sub_tasks( index.row() );
}

time_t TrackedTasksListModel::seconds_today( const QModelIndex &index ) const
{
    if( index.row() < 0 || index.row() >= _task_search_results.size() )
        return -1;

    return 123; //TODO
}


time_t TrackedTasksListModel::seconds_last_active( const QModelIndex &index ) const
{
    if( index.row() < 0 || index.row() >= _task_search_results.size() )
        return -1;

	return time(NULL) - _task_search_results[ index.row() ].last_logged_time;
}
