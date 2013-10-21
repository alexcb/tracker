#include <string>
#include <vector>

#include <QtCore\qobject.h>

class UserSettings : public QObject
{
	Q_OBJECT
public:
	UserSettings();

	void load( const char *file_path );
	void save();

	int num_minutes_before_idle;
	int auto_complete_show_tasks_newer_than_days;

	std::vector<std::string> auto_complete_pre_populated_tasks;

#ifdef WIN32
	std::string hot_key_code;
#endif

signals:
	void settingsChanged(); 

private:
	std::string _file_path;
};
