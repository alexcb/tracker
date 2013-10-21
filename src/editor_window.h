#ifndef _EDITOR_WINDOW_H_DFGSNXLV
#define _EDITOR_WINDOW_H_DFGSNXLV

#include <QMainWindow>

class EditorWidget;
class TaskList;
class UserSettings;

class EditorWindow : public QMainWindow
{
	Q_OBJECT //if we include this, we need to use the moc tool to generate some c++ code for us. ugh.

public:
	EditorWindow(TaskList *tasks, UserSettings *user_settings, QWidget *parent = 0);
	EditorWidget *editor_widget;
};

#endif //_EDITOR_WINDOW_H_DFGSNXLV