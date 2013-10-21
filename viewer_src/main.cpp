#include <Windows.h>

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include "QtGui\qevent.h"

#include "editor_window.h"
#include "editor_widget.h"

#include "task_week_editor.h"

#include "task_list.h"

#include "task_entry_window.h"

#include "user_settings.h"


//globals

//the tasks
TaskList tasks;

//the editor window
EditorWindow *task_editor_window;

//the task entry window
TaskEntryWindow *task_entry_window;

UserSettings user_settings;

int CALLBACK WinMain(
  _In_  HINSTANCE hInstance,
  _In_  HINSTANCE hPrevInstance,
  _In_  LPSTR lpCmdLine,
  _In_  int nCmdShow
)
{
	int argc = 0;
	QApplication app(argc, NULL);
	
	tasks.load( getTrackerTaskFile().c_str() );
	user_settings.load( getTrackerUserSettingsFile().c_str() );

	//create viewer window/GUI
	task_editor_window = new EditorWindow( &tasks, &user_settings );
	TaskWeekEditor *task_editor = task_editor_window->editor_widget->task_week_editor;
	
	const unsigned int num_tasks = tasks.num_logged_tasks();
	if( tasks.num_logged_tasks() > 1 ) {
		task_editor->setWeek(tasks.getLoggedTask( num_tasks - 1 )->time );
	}

	//task_editor->setTaskChangedCallback( task_changed, NULL );
	task_editor_window->resize(900, 900);

	//create task entry GUI
	task_entry_window = new TaskEntryWindow( &tasks, &user_settings, task_editor_window );
	task_entry_window->resize(700, 100);
	task_entry_window->show();

    return app.exec();
}

