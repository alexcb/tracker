#include "editor_window.h"

#include <QWidget>
#include <QMenu>
#include <QMenuBar>

#include "task_week_editor.h"

#include "editor_widget.h"


EditorWindow::EditorWindow(TaskList *tasks, UserSettings *user_settings, QWidget *parent)
    : QMainWindow(parent)
{
	editor_widget = new EditorWidget(tasks, user_settings, this);
	setCentralWidget(editor_widget);

	QAction *undo = new QAction("&Undo", this);
	undo->setShortcut(tr("CTRL+Z"));

	QMenu *file;
	file = menuBar()->addMenu( "&File" );
	file->addAction( undo );


	connect( undo, SIGNAL( triggered() ), editor_widget->task_week_editor, SLOT( undo() ) );
}
