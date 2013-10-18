#include "editor_window.h"

#include <QWidget>

#include "task_week_editor.h"

#include "editor_widget.h"


EditorWindow::EditorWindow(TaskList *tasks, UserSettings *user_settings, QWidget *parent)
    : QMainWindow(parent)
{
	editor_widget = new EditorWidget(tasks, user_settings, this);
	setCentralWidget(editor_widget);
}

void EditorWindow::fileSave()
{
	//TODO THIS NEEDS TO BE IMPLEMENTED
}
