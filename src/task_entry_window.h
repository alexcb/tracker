#ifndef TASK_ENTRY_WINDOW_H_FSDFNZ
#define TASK_ENTRY_WINDOW_H_FSDFNZ

#include <QMainWindow>

#include <string>

class TaskEntryWidget;
class TaskList;
class EditorWindow;
class UserSettings;

class TaskEntryWindow : public QMainWindow
{
	Q_OBJECT

public:
	TaskEntryWindow( TaskList *task_list, UserSettings *user_settings, EditorWindow *editor_window, QWidget *parent = 0 );
	TaskEntryWidget *_task_entry_widget;

	void keyPressEvent(QKeyEvent *event);

#ifdef _WIN32
	virtual bool nativeEvent (const QByteArray &eventType, void *message, long *result );
	void registerHotKey();
#endif //_WIN32

private slots:

#ifdef _WIN32
	void hotKeyChanged();
#endif //_WIN32

protected:
	UserSettings *_user_settings;
#ifdef _WIN32
	std::string _current_registered_hot_key;
#endif //_WIN32
};

#endif //TASK_ENTRY_WINDOW_H_FSDFNZ