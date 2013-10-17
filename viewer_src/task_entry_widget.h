#ifndef _TASK_ENTRY_WIDGET_H_SFNX
#define _TASK_ENTRY_WIDGET_H_SFNX

#include <QWidget>

class QLineEdit;
class QLabel;
class QTimer;

class TaskDropDownWidget;
class TrackedTasksListModel;
class TaskList;
class EditorWindow;
class UserSettings;

class TaskEntryWidget : public QWidget
{
	Q_OBJECT //if we include this, we need to use the moc tool to generate some c++ code for us. ugh.
public:
    TaskEntryWidget( TaskList *task_list, UserSettings *user_settings, EditorWindow *editor_window, QWidget *parent = 0);

	void promptTask();
	//void promptIdleTask( time_t seconds_idle );

protected:
	void updateWidgets();

private slots:
	void newTaskEntered();
	void showTaskEditor();
	void showSettingsDialog();
	void timerUpdate();

private:
	QLineEdit *_task_edit;
	QLabel *_current_task_label;
	TaskDropDownWidget *_drop_down_widget;

	QLabel *_idle_label_1;
	QLabel *_idle_label_2;

	TaskList *_task_list;
	EditorWindow *_editor_window;

	TrackedTasksListModel *_tracked_tasks_list_model;

	time_t _went_idle_at;
	bool _displayed_idle_return_window;
	QTimer *_timer;

	UserSettings *_user_settings;
};

#endif //_TASK_ENTRY_WIDGET_H_SFNX