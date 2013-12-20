#include "task_list_editor_window.h"

#include <QWidget>
#include <QMenu>
#include <QMenuBar>

#include "task_list_editor_widget.h"


TaskListEditorWindow::TaskListEditorWindow(TaskList *tasks, UserSettings *user_settings, QWidget *parent)
    : QMainWindow(parent)
{
	task_week_editor = new TaskListEditorWidget( tasks, user_settings, this );
	setCentralWidget( task_week_editor );

//FIXME displaying a menu bar causes the window to be displayed before show() is called
#ifdef WIN32
	QAction *undo = new QAction("&Undo", this);
	undo->setShortcut(tr("CTRL+Z"));

	QMenu *file;
	file = menuBar()->addMenu( "&File" );
	file->addAction( undo );

	connect( undo, SIGNAL( triggered() ), task_week_editor, SLOT( undo() ) );
#endif
}
