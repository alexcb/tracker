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

	QAction *undo = new QAction("&Undo", this);
	undo->setShortcut(tr("CTRL+Z"));

	QMenu *file;
	file = menuBar()->addMenu( "&File" );
	file->addAction( undo );

	connect( undo, SIGNAL( triggered() ), task_week_editor, SLOT( undo() ) );
}
