#ifndef _EDITOR_WIDGET_H_FSDMXN
#define _EDITOR_WIDGET_H_FSDMXN

#include <QWidget>

class TaskWeekEditor;
class TaskDropDownWidget;
class TaskList;
class UserSettings;

class EditorWidget : public QWidget
{
public:
	EditorWidget(TaskList *tasks, UserSettings *user_settings, QWidget *parent = 0);
	TaskWeekEditor *task_week_editor;
};

#endif //_EDITOR_WIDGET_H_FSDMXN