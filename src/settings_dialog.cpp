#include "settings_dialog.h"

#include "user_settings.h"

#include <QPushButton>
#include <QLayout>
#include <QLineEdit>
#include <QLabel>
#include <QIntValidator>

SettingsDialog::SettingsDialog( UserSettings *user_settings, QWidget *parent, Qt::WindowFlags f ) :
	QDialog( parent, f ),
	_user_settings( user_settings )
{
#ifdef WIN32
	_hotkey = new QLineEdit(this);
	_hotkey->setText( _user_settings->hot_key_code.c_str() );
#endif //WIN32

	_minutes_before_idle = new QLineEdit(this);
	_minutes_before_idle->setValidator( new QIntValidator(0, 24*60, this) );
	_minutes_before_idle->setText( QVariant( _user_settings->num_minutes_before_idle ).toString() );

	_hide_tasks_older_than_days = new QLineEdit(this);
	_minutes_before_idle->setValidator( new QIntValidator(0, 365, this) );
	_hide_tasks_older_than_days->setText( QVariant( _user_settings->auto_complete_show_tasks_newer_than_days ).toString() );

	// setup layout
	QVBoxLayout *vbox = new QVBoxLayout(this);
	vbox->setSpacing(1);

#ifdef WIN32
	vbox->addWidget( new QLabel("Hot key (expected format: ctrl+shit+alt+k") );
	vbox->addWidget( _hotkey );
#endif //WIN32

	vbox->addWidget( new QLabel("minutes before going into idle mode") );
	vbox->addWidget( _minutes_before_idle );

	vbox->addWidget( new QLabel("Hide auto completion on tasks older than this many days") );
	vbox->addWidget( _hide_tasks_older_than_days );

	QPushButton *settings_button = new QPushButton("Save Settings", this);
	connect( settings_button, SIGNAL(clicked()), this, SLOT(saveSettings()));
	vbox->addWidget( settings_button );

	setLayout(vbox);
}

void SettingsDialog::saveSettings()
{
	//idle time
	_user_settings->num_minutes_before_idle = _minutes_before_idle->text().toInt();

	//hide tasks this old
	_user_settings->auto_complete_show_tasks_newer_than_days = _hide_tasks_older_than_days->text().toInt();

#ifdef WIN32
	//hot key
	_user_settings->hot_key_code = _hotkey->text().toUtf8().constData();
#endif //WIN32

	_user_settings->save();

	close();
}
