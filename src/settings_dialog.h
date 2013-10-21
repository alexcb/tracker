#include <QDialog>

class QLineEdit;
class UserSettings;

class SettingsDialog : public QDialog
{
	Q_OBJECT //if we include this, we need to use the moc tool to generate some c++ code for us. ugh.
public:
	explicit SettingsDialog(UserSettings *user_settings, QWidget *parent = 0, Qt::WindowFlags f = 0);
private slots:
	void saveSettings();
private:
	UserSettings *_user_settings;
	QLineEdit *_hotkey;
	QLineEdit *_minutes_before_idle;
	QLineEdit *_hide_tasks_older_than_days;
};
