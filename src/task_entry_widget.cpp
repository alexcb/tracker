#include "task_entry_widget.h"

#include "tracked_tasks_list_model.h"
#include "tracked_task_item_delegate.h"
#include "tracked_task_completer.h"
#include "tracked_task_line_edit.h"
#include "task_list_editor_window.h"
#include "task_list.h"
#include "human_text.h"
#include "idle_input_time.h"
#include "user_settings.h"

#include "settings_dialog.h"

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QTimer>

#include <QComboBox>
#include <QAbstractItemView>

#include <algorithm>
#include <sstream>
#include <time.h>

TaskEntryWidget::TaskEntryWidget( TaskList *task_list, UserSettings *user_settings, TaskListEditorWindow *editor_window, QWidget *parent ) :
	QWidget( parent ),
	_task_list( task_list ),
	_editor_window( editor_window ),
	_went_idle_at( 0 ),
	_displayed_idle_return_window( false ),
	_user_settings( user_settings )
{
	_current_task_label = new QLabel( this );

	_task_edit = new TrackedTaskLineEdit( task_list, user_settings, this );
	connect( _task_edit, SIGNAL( returnPressed() ), this, SLOT( newTaskEntered() ) );
	
	_idle_label_1 = new QLabel("Idle activity dectected: ", this);
	_idle_label_2 = new QLabel("", this);

	_idle_label_1->hide();
	_idle_label_2->hide();

	QFormLayout *layout = new QFormLayout( this );
	layout->addRow( _idle_label_1, _idle_label_2 );
	layout->addRow( new QLabel("Current task: ", this), _current_task_label );
	layout->addRow( new QLabel("Task: ", this), _task_edit );

	QPushButton *edit_button = new QPushButton("task editor", this);
	connect( edit_button, SIGNAL(clicked()), this, SLOT(showTaskEditor()));

	QPushButton *settings_button = new QPushButton( "settings", this );
	connect( settings_button, SIGNAL( clicked() ), this, SLOT( showSettingsDialog() ) );

	//attach buttons to grid (side by side)
	QHBoxLayout *horiz_layout = new QHBoxLayout(this);
	horiz_layout->addWidget( settings_button );
	horiz_layout->addWidget( edit_button );
	layout->addRow( NULL, horiz_layout );

	setLayout(layout);

	_timer = new QTimer(this);
	connect( _timer, SIGNAL(timeout()), this, SLOT(timerUpdate()) );
	_timer->setSingleShot( true );
	_timer->start( 1000 );

	updateWidgets();
}

void TaskEntryWidget::updateWidgets()
{
	try {
		LoggedTask current_task = _task_list->getCurrentTask();
		time_t duration = time(NULL) - current_task.time;
		const char *task_name = current_task.task->name.c_str();

		std::string duration_str;
		if( duration > 60*60 ) {
			duration_str = std::string("approximately ") + human_time_elapsed(duration, true);
		} else {
			duration_str = human_time_elapsed(duration);
		}

		//set current task label
		std::ostringstream oss;
		oss << task_name << " (logged " << duration_str << ")";
		_current_task_label->setText( oss.str().c_str() );

		//auto fill current task in line edit
		_task_edit->setText( task_name );
		_task_edit->selectAll();

	} catch( const NoTaskHistoryError& e ) {
		//first time running (no task history)
		_current_task_label->setText( "No logged tasks history" );
	}

	if( _went_idle_at > 0 ) {
		time_t seconds_idle = time(NULL) - _went_idle_at;

		std::ostringstream oss;
		oss << "What were you working on for the past " << human_time_elapsed(seconds_idle) << "?";

		_idle_label_2->setText( oss.str().c_str() );

		_idle_label_1->show();
		_idle_label_2->show();
	} else {
		_idle_label_1->hide();
		_idle_label_2->hide();
	}
}

void TaskEntryWidget::promptTask()
{
	updateWidgets();

	Qt::WindowFlags eFlags = parentWidget()->windowFlags ();
	eFlags |= Qt::WindowStaysOnTopHint;
	parentWidget()->setWindowFlags(eFlags);
	
	parentWidget()->showNormal();
	parentWidget()->show();
	parentWidget()->raise();

    //Qt::WindowFlags eFlags = windowFlags ();
    //eFlags |= Qt::WindowStaysOnTopHint;
    //setWindowFlags(eFlags);

	_task_edit->setFocus();
}


void TaskEntryWidget::newTaskEntered()
{
	time_t task_start_time = _went_idle_at > 0 ? _went_idle_at : time(NULL);
	_task_list->addLoggedTask( _task_edit->text().toUtf8().constData(), task_start_time );
	_task_list->save();
	parentWidget()->hide();
	_went_idle_at = 0;
}

void TaskEntryWidget::showTaskEditor()
{
	_editor_window->show();
}

void TaskEntryWidget::showSettingsDialog()
{
	SettingsDialog dl( _user_settings, this );
	dl.exec();
}

void TaskEntryWidget::timerUpdate()
{
	time_t ms_idle = getMilliSecondsSinceLastInput();

	const int idle_time_ms = _user_settings->num_minutes_before_idle * 60 * 1000;
	time_t next_check = 200;
	if( ms_idle >= idle_time_ms ) {
		//detected idle
		if( _went_idle_at == 0 ) {
			_went_idle_at = time(NULL);
			//_displayed_idle_return_window = false;
		}
	} else {
		next_check = std::min( idle_time_ms - ms_idle, static_cast<time_t>(60 * 1000) );
		if( _went_idle_at > 0 /*&& _displayed_idle_return_window == false*/ ) {
			//user returned
			promptTask();
			//_displayed_idle_return_window = true;
		}
	}
	
	_timer->start( next_check );
}
