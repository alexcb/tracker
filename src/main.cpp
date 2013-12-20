#include "task_list_editor_window.h"
#include "task_list_editor_widget.h"
#include "task_list.h"
#include "task_entry_window.h"
#include "user_settings.h"
#include "inter_process_wakeup.h"

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QEvent>
#include <QMessageBox> //TODO remove
#include <iostream>

#ifdef WIN32
#include <Windows.h>
#endif //WIN32

/////////////////////////////////
//globals

//the tasks
TaskList tasks;

//the editor window
TaskListEditorWindow *task_editor_window = NULL;

//the task entry window
TaskEntryWindow *task_entry_window = NULL;

UserSettings user_settings;

/////////////////////////////////

#ifndef WIN32
void show_tracker_window()
{
	if( task_entry_window )
		task_entry_window->show();
}

//returns true if this process needs to shutdown
bool signalOtherInstanceOrRegisterInstance()
{
	std::string fname = getPIDFilePath();
	pid_t pid = readPID( fname.c_str() );
	if( signalOtherInstance( pid ) == true ) {
        //other process was signaled, so this instance should shutdown
		return true;
	}

	if( setupWakeupHandler( show_tracker_window ) == false ) {
        std::cerr << "Parent: Unable to create handler for SIGUSR1" << std::endl;
    }
    
    writePID( fname.c_str() );
	return false;
}
#endif // not WIN32

#ifdef WIN32
int CALLBACK WinMain(
  _In_  HINSTANCE hInstance,
  _In_  HINSTANCE hPrevInstance,
  _In_  LPSTR lpCmdLine,
  _In_  int nCmdShow
)
#else
int main(int argc, char** argv, char** env)
#endif
{
#ifdef WIN32
	int argc = 0;
#else
	if( signalOtherInstanceOrRegisterInstance() ) {
		// Other process is running and has been signaled
		return 0;
	}
#endif
	QApplication app(argc, NULL);

	tasks.load( getTrackerTaskFile().c_str() );
	user_settings.load( getTrackerUserSettingsFile().c_str() );

	//create viewer window/GUI
	task_editor_window = new TaskListEditorWindow( &tasks, &user_settings );
	TaskListEditorWidget *task_editor = task_editor_window->task_week_editor;

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

