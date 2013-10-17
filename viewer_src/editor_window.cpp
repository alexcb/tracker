#include "editor_window.h"

#include <QApplication>
#include <QWidget>
#include <QMenu>
#include <QMenuBar>
#include <QVBoxLayout>

#include "task_week_editor.h"

#include "editor_widget.h"


EditorWindow::EditorWindow(TaskList *tasks, UserSettings *user_settings, QWidget *parent)
    : QMainWindow(parent)
{
	QAction *save = new QAction("&Save", this);
	save->setShortcut(tr("CTRL+S"));

	QMenu *file_menu;
	file_menu = menuBar()->addMenu("&File");
	file_menu->addAction(save);

	editor_widget = new EditorWidget(tasks, user_settings, this);

	setCentralWidget(editor_widget);

	connect(save, SIGNAL(triggered()), this, SLOT(fileSave()));
}

void EditorWindow::fileSave()
{
	//TODO THIS NEEDS TO BE IMPLEMENTED
}
