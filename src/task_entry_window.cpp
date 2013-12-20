#include "task_entry_window.h"

#include "task_entry_widget.h"
#include "user_settings.h"

#include <QKeyEvent>

#ifdef _WIN32
#include <Windows.h>
#endif //_WIN32

TaskEntryWindow::TaskEntryWindow( TaskList *task_list, UserSettings *user_settings, TaskListEditorWindow *editor_window, QWidget *parent ) : 
	QMainWindow(parent),
	_user_settings( user_settings )
{
	setWindowTitle("task tracker");

	_task_entry_widget = new TaskEntryWidget( task_list, user_settings, editor_window, this );
	setCentralWidget( _task_entry_widget );

#ifdef _WIN32
	registerHotKey();
	connect( user_settings, SIGNAL( settingsChanged() ), this, SLOT( hotKeyChanged() ) );
#endif //_WIN32
}

void TaskEntryWindow::keyPressEvent(QKeyEvent *event)
{
	if( event->key() == Qt::Key_Escape ) {
		hide();
	} else {
		QMainWindow::keyPressEvent( event );
	}
}


void TaskEntryWindow::promptTask()
{
	_task_entry_widget->promptTask();
}

void TaskEntryWindow::closeEvent( QCloseEvent *event )
{
	// Hide this window rather than closing it
	event->ignore();
	hide();
}


#ifdef _WIN32

bool TaskEntryWindow::nativeEvent( const QByteArray & eventType, void * message, long * result )
{
	MSG* msg = reinterpret_cast<MSG*>(message);

	if( msg->message == WM_HOTKEY ) {
		show();
		raise();
		showNormal();
		_task_entry_widget->promptTask();
	}
	return false;
}

void TaskEntryWindow::registerHotKey()
{
	if( _current_registered_hot_key == _user_settings->hot_key_code )
		return;

	if( _current_registered_hot_key != "" )
		UnregisterHotKey( (HWND)winId(), 1 );

	_current_registered_hot_key = _user_settings->hot_key_code;

	UINT modifiers = 0;
	if( strstr( _user_settings->hot_key_code.c_str(), "ctrl") )
		modifiers = modifiers | MOD_CONTROL;
	if( strstr( _user_settings->hot_key_code.c_str(), "shift") )
		modifiers = modifiers | MOD_SHIFT;
	if( strstr( _user_settings->hot_key_code.c_str(), "alt") )
		modifiers = modifiers | MOD_ALT;

	int len = _user_settings->hot_key_code.length();
	if( len > 0 ) {
		char key = _user_settings->hot_key_code[ len-1 ];
		key = toupper(key);

		BOOL registerd = RegisterHotKey( (HWND)winId(), 1, modifiers, key);
	}
}

void TaskEntryWindow::hotKeyChanged()
{
	registerHotKey();
}

#endif //_WIN32
