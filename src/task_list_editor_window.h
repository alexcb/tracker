#ifndef _EDITOR_WINDOW_H_DFGSNXLV
#define _EDITOR_WINDOW_H_DFGSNXLV

#include <QMainWindow>

class TaskListEditorWidget;
class TaskList;
class UserSettings;

class TaskListEditorWindow : public QMainWindow
{
	Q_OBJECT

public:
	TaskListEditorWindow(TaskList *tasks, UserSettings *user_settings, QWidget *parent = 0);
	TaskListEditorWidget *task_week_editor;
};

#endif //_EDITOR_WINDOW_H_DFGSNXLV