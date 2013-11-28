#include <QLineEdit>

class QAbstractItemDelegate;
class TrackedTasksListModel;
class TaskList;
class UserSettings;
class QCompleter;

class TrackedTaskLineEdit : public QLineEdit
{
public:
	TrackedTaskLineEdit( TaskList *task_list, UserSettings *user_settings, QWidget *parent );
	~TrackedTaskLineEdit();

	virtual void keyPressEvent( QKeyEvent * e );
	virtual bool event( QEvent *event );
protected:
public: //TODO REMOVE THIS
	bool _current_prefix_match;
	void selectCompletion();
	TrackedTasksListModel *_tracked_tasks_list_model;
	QAbstractItemDelegate *_item_delegate;
	QCompleter *_completer;
};
