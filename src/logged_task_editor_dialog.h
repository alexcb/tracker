#include <QDialog>

class LoggedTask;
class TaskList;
class TrackedTasksListModel;
class TrackedTaskLineEdit;
class UserSettings;

class LoggedTaskEditorDialog : public QDialog
{
	Q_OBJECT
public:
	explicit LoggedTaskEditorDialog(LoggedTask *logged_task, TaskList *task_list, UserSettings *user_settings, QWidget *parent = 0, Qt::WindowFlags f = 0);
private slots:
	void renameSingleLoggedTask();
	void renameAllLoggedTasks();
private:
	LoggedTask *_logged_task;
	TaskList *_task_list;

	TrackedTasksListModel *_tracked_tasks_list_model;
	TrackedTaskLineEdit *_task_edit;
};
