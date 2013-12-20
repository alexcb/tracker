#include "logged_task_editor_dialog.h"

#include "task_list.h"
#include "tracked_task_line_edit.h"

#include <QPushButton>
#include <QLabel>
#include <QFormLayout>

#include <sstream>

LoggedTaskEditorDialog::LoggedTaskEditorDialog( LoggedTask *logged_task, TaskList *task_list, UserSettings *user_settings, QWidget *parent, Qt::WindowFlags f ) :
	QDialog( parent, f ),
	_logged_task( logged_task ),
	_task_list( task_list )
{
	_task_edit = new TrackedTaskLineEdit( task_list, user_settings, this );
	connect( _task_edit, SIGNAL( returnPressed() ), this, SLOT( newTaskEntered() ) );

	QFormLayout *layout = new QFormLayout( this );

	layout->addRow( new QLabel( "old task name:" ), new QLabel( logged_task->task->name.c_str() ) );
	layout->addRow( new QLabel( "new task name:" ), _task_edit );

	QPushButton *rename_single_task_button = new QPushButton( "rename single instance of task", this );
	connect( rename_single_task_button, SIGNAL( clicked() ), this, SLOT( renameSingleLoggedTask() ) );
	layout->addWidget( rename_single_task_button );

	unsigned int num_logged_tasks = _task_list->num_logged_tasks( _logged_task->task );
	std::ostringstream oss;
	oss << "rename all " << num_logged_tasks << " instances of same-named task";
	QPushButton *rename_all_tasks_button = new QPushButton( oss.str().c_str(), this );
	connect( rename_all_tasks_button, SIGNAL( clicked() ), this, SLOT( renameAllLoggedTasks() ) );
	layout->addWidget( rename_all_tasks_button );

	setLayout( layout );
	resize( 500, 200 );
}

void LoggedTaskEditorDialog::renameSingleLoggedTask()
{
	_task_list->renameLoggedTask( _logged_task, _task_edit->text().toUtf8().constData(), false );
	_task_list->save();
	close();
}

void LoggedTaskEditorDialog::renameAllLoggedTasks()
{
	_task_list->renameLoggedTask( _logged_task, _task_edit->text().toUtf8().constData(), true );
	_task_list->save();
	close();
}
