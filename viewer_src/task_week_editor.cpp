#include "task_week_editor.h"

#include "math_utils.h"
#include "logged_task_editor_dialog.h"

#include "task_list.h"

#include <QtWidgets>
#include <QPushButton>
#include <qtooltip.h>

#include <time.h>
#include <assert.h>

#include <iostream>
#include <sstream>

const int SECONDS_PER_DAY = 60 * 60 * 24;
const int NUM_DAYS = 7;

inline bool taskVisibleInTimeSpan( time_t task_start_utc, time_t task_end_utc, time_t time_start_utc, time_t time_end_utc )
{
	return time_start_utc <= task_end_utc && task_start_utc <= time_end_utc;
}

inline QString formatTaskStartTime( time_t time_start )
{
	QDateTime date_time;
	date_time.setTime_t( time_start );
	return date_time.toString( "ddd HH:mm" );
}

inline QString seconds_to_hours( int seconds )
{
	std::ostringstream oss;
	int minutes = seconds / 60;
	int hours = minutes / 60;
	minutes = minutes % 60;
	if( hours > 1 ) 
		oss << hours << " hours ";
	else if( hours == 1 )
		oss << "1 hour ";
	if( minutes > 1 ) 
		oss << minutes << " mins";
	else
		oss << "1 min";
	return QString( oss.str().c_str() );
}

//t - epoch based UTC time
//returns seconds since midnight in local time
inline time_t getSecondsSinceLocalTimeMidnight( time_t t )
{
	QDateTime date_time;
	date_time.setTime_t( t );
	return QTime(0,0).secsTo( date_time.time() );
}

TaskWeekEditor::TaskWeekEditor( TaskList *tasks, UserSettings *user_settings, QWidget *parent )
    : QWidget( parent ),
	_margin( 20 ),
	_top_heading_size( 40 ),
	_day_start_time( 0 ),
	_day_end_time( 60*60*24 - 1 ),
	_time_column_width( 50 ),
	_selected_boundary_task( NULL ),
	_selected_task( NULL ),
	_selected_visible_task( NULL ),
	_task_changed_cb( NULL ),
	_task_changed_cb_data( NULL ),
	_tasks( tasks ),
	_user_settings( user_settings )
{
	_prev_week_button = new QPushButton( "prev week", this );
	_this_week_button = new QPushButton( "this week", this );
	_next_week_button = new QPushButton( "next week", this );

	connect( _prev_week_button, SIGNAL( clicked() ), this, SLOT( handleButton() ) );
	connect( _next_week_button, SIGNAL( clicked() ), this, SLOT( handleButton() ) );
	connect( _this_week_button, SIGNAL( clicked() ), this, SLOT( handleButton() ) );

	_task_name_line_edit = new QLineEdit( this );

	setMinimumSize( 700, 200 );

	setMouseTracking( true );
}

void TaskWeekEditor::setWeek( time_t time )
{
	QDateTime date_time;
	date_time.setTime_t( time );
	date_time.setTime( QTime( 0, 0 ) );

	//change day to sunday 0:0:0
	int gah = date_time.date().dayOfWeek();
	int before = date_time.toTime_t();
	date_time = date_time.addDays( -gah );
	_week_start_time = date_time.toTime_t();

	//change to saturday 23:59:59
	date_time = date_time.addDays( 6 );
	date_time.setTime( QTime(23, 59, 59) );
	_week_end_time = date_time.toTime_t();
}

void TaskWeekEditor::calcWindowResizeValues()
{
	_grid_width = width() - _margin * 2;
	_grid_height = height() - _margin * 2 - _top_heading_size;

	_day_column_width = (_grid_width - _time_column_width) / NUM_DAYS;
}

//TODO check how this handles DST (setting to 2hr 30min on a fall back day, I would want the time to then be 1:30)
time_t TaskWeekEditor::localDayTimeToUTC( int day_of_week, int seconds_since_midnight ) const
{
	QDateTime date_time;
	date_time.setTime_t( _week_start_time );
	date_time = date_time.addDays( day_of_week ).addSecs( seconds_since_midnight );
	return date_time.toTime_t();
}

void TaskWeekEditor::calcVisibleTasks()
{
	_visible_tasks.clear();

	const int num_items = _tasks->num_logged_tasks();
	for( int i = 0; i < num_items; i++ ) {

		LoggedTask *task = _tasks->getLoggedTask(i);
		LoggedTask *next_task = ( i + 1 < num_items ? _tasks->getLoggedTask(i+1) : NULL );

		//global (epoch) start/end times
		time_t task_start_time = task->time; //_tasks[i].time_start;
		time_t task_end_time = ( next_task ? next_task->time : time(NULL) );

		if( taskVisibleInTimeSpan( task_start_time, task_end_time, _week_start_time, _week_end_time ) ) {

			for( int day_of_week = 0; day_of_week < 7; day_of_week++ ) {

				//TODO this likely wont work for day light saving days
				time_t display_start = localDayTimeToUTC( day_of_week, _day_start_time );
				time_t display_end = localDayTimeToUTC( day_of_week, _day_end_time );

				if( taskVisibleInTimeSpan( task_start_time, task_end_time, display_start, display_end ) ) {
			
					VisibleTask time_range;
					time_range.task = task;
					time_range.task_i = i;
					//if( time_range.task->label == "interview prep" ) {
					//	int breaker = 1231;
					//	OutputDebugStringA("seen it\n");
					//}
					//TODO figure something out for time changes
					time_range.daylight_saving = 0;
					time_range.day_of_week = day_of_week;

					if( task_start_time < display_start ) {
						//start time of task is before the day being displayed
						time_range.start_y = _margin + _top_heading_size;
						time_range.continued_task = true;
					} else {
						//task start time is being displayed
						const time_t secs_since_midnight_local = getSecondsSinceLocalTimeMidnight( task_start_time );
						const float start_pct = static_cast<float>( secs_since_midnight_local - _day_start_time ) / ( _day_end_time - _day_start_time );
						time_range.start_y = std::max( static_cast<int>( start_pct * _grid_height), 0 ) + _margin + _top_heading_size;
						time_range.continued_task = false;
					}

					time_range.task_seconds = task_end_time - task_start_time;
					//time_range.task_seconds = ( i + 1 >= num_items ? time(NULL) : _tasks[i+1].time_start ) - _tasks[i].time_start;
					time_range.in_progress = i == num_items - 1;
					_visible_tasks.push_back( time_range );
					break;
				}
			}
		}
	}

	//calculate the stop_y for each displayed task
	int num_displayed_items = _visible_tasks.size();
	for( int i = 0; i < num_displayed_items; i++ ) {
		if( i + 1 < num_displayed_items ) {
			//next item exists

			while( _visible_tasks[i].day_of_week < _visible_tasks[i+1].day_of_week ) {
				VisibleTask tmp = _visible_tasks[i];
				//set current span to end of day
				_visible_tasks[i].stop_y = _margin + _top_heading_size + _grid_height;
				//and insert a new item for the next day
				i++;
				tmp.day_of_week++;
				tmp.start_y = _margin + _top_heading_size;
				tmp.continued_task = true;
				_visible_tasks.insert( _visible_tasks.begin() + i, tmp );
				num_displayed_items++;
			}

			_visible_tasks[i].stop_y = _visible_tasks[i+1].start_y;

		} else {
			//on the last item (either current item, or spans into a future date)
			//TODO end time not set correctly on current tasks (must use time(NULL), but be careful that it doesnt span a day)
			const time_t secs_since_midnight_local = _day_end_time; //( _visible_tasks[i].in_progress ? getSecondsSinceLocalTimeMidnight(time(NULL)) : _day_end_time );
			const float start_pct = static_cast<float>( secs_since_midnight_local - _day_start_time ) / ( _day_end_time - _day_start_time );
			_visible_tasks[i].stop_y = static_cast<int>( start_pct * _grid_height ) + _margin + _top_heading_size;
		}
	}
}

int TaskWeekEditor::getSecondsPerPlotTick( int tick_spacing ) const
{
	const int num_ticks = _grid_height / tick_spacing;
	const int day_seconds_displayed = _day_end_time - _day_start_time;
	int seconds_per_tick = -1;
	if( num_ticks > 1 ) {
		seconds_per_tick = day_seconds_displayed / num_ticks;

		//anything over 30 min, round to the hour
		if( seconds_per_tick > 30*60 )
			seconds_per_tick = round_up( seconds_per_tick, 60*60 );
		//over 10 min, round to 15 min
		else if( seconds_per_tick > 10*60 )
			seconds_per_tick = round_up( seconds_per_tick, 15*60 );
		//over 5 min, round to 10 min
		else if( seconds_per_tick > 5*60 )
			seconds_per_tick = round_up( seconds_per_tick, 10*60 );
		//over 5 min, round to 10 min
		else if( seconds_per_tick > 60 )
			seconds_per_tick = round_up( seconds_per_tick, 5*60 );
		else
			seconds_per_tick = round_up( seconds_per_tick, 60 );
	}
	return seconds_per_tick;
}

LoggedTask* TaskWeekEditor::getTaskByStartPos( int x, int y, int y_tolerance )
{
	LoggedTask *task = NULL;
	const int day_of_week = dayOfWeekByPos(x);
	const int num_visible_tasks = _visible_tasks.size();
	for( int i = 0; i < num_visible_tasks && task == NULL; i++ ) {
		if( _visible_tasks[i].day_of_week == day_of_week && 
			approx_equal( _visible_tasks[i].start_y, y, y_tolerance ) && 
			_visible_tasks[i].continued_task == false
		) {
			task = _visible_tasks[i].task;
		}
	}
	return task;
}

LoggedTask* TaskWeekEditor::getTaskByPos( int x, int y )
{
	VisibleTask *visible_task = getVisibleTaskByPos( x, y );
	LoggedTask *task = NULL;
	if( visible_task ) {
		task = visible_task->task;
	}
	return task;
}

TaskWeekEditor::VisibleTask* TaskWeekEditor::getVisibleTaskByPos( int x, int y )
{
	VisibleTask *visible_task = NULL;
	const int day_of_week = dayOfWeekByPos(x);
	const int num_visible_tasks = _visible_tasks.size();
	for( int i = 0; i < num_visible_tasks && visible_task == NULL; i++ ) {
		if( _visible_tasks[i].day_of_week == day_of_week && 
			_visible_tasks[i].start_y < y && 
			_visible_tasks[i].stop_y > y
		) {
			visible_task = &_visible_tasks[i];
		}
	}
	return visible_task;
	
}

time_t TaskWeekEditor::mousePosToTimestamp( int x, int y ) const
{
	const int day_of_week = dayOfWeekByPos(x);
	
	const float grid_y_pct = static_cast<float>(y - _margin - _top_heading_size) / _grid_height;
	const int displayed_seconds = _day_end_time - _day_start_time;
	const int seconds_since_midnight_local = displayed_seconds * grid_y_pct + _day_start_time;
	const time_t timestamp = _week_start_time + day_of_week * SECONDS_PER_DAY + seconds_since_midnight_local;

	return timestamp;
}

void TaskWeekEditor::setTaskTimestamp( LoggedTask *task, time_t timestamp, time_t min_time_padding )
{
	LoggedTask *start_task = _tasks->getLoggedTask(0);
	LoggedTask *end_task = start_task + _tasks->num_logged_tasks();

	LoggedTask *prev_task = task - 1;
	LoggedTask *next_task = task + 1;
	if( prev_task < start_task )
		prev_task = NULL;
	if( next_task >= end_task )
		next_task = NULL;

	time_t min_timestamp = prev_task ? prev_task->time : 0;
	time_t max_timestamp = next_task ? next_task->time : time(NULL);

	min_time_padding = std::min( (max_timestamp - min_timestamp) / 4, min_time_padding);

	timestamp = std::max(timestamp, min_timestamp + min_time_padding);
	timestamp = std::min(timestamp, max_timestamp - min_time_padding);

	task->time = timestamp;

	//TODO only touch neighbouring tasks
	calcVisibleTasks();
	update();
}

//////////////////////////////////
//QT EVENTS BELOW

void TaskWeekEditor::resizeEvent( QResizeEvent *event )
{
	calcWindowResizeValues();

	int y = pos().ry();

	_prev_week_button->setGeometry( QRect( QPoint(0, 0), QSize(100, 20) ) );
	_next_week_button->setGeometry( QRect( QPoint(width()-100, 0 ), QSize(100, 20) ) );
	_this_week_button->setGeometry( QRect( QPoint((width()-100)/2, 0 ), QSize(100, 20) ) );
	
	setTaskEditorPosition();

	//TODO only adjust sizes, not scan for events as the times have not changed
	calcVisibleTasks();
}

void TaskWeekEditor::setTaskEditorPosition()
{
	if( _selected_visible_task ) {
		const int task_height = _selected_visible_task->stop_y - _selected_visible_task->start_y;

		const int x = _selected_visible_task->day_of_week * _day_column_width + _margin + _time_column_width;
		const int y = _selected_visible_task->start_y + task_height / 2 - 20;
		
		const int height = 20;
		const int width = _day_column_width;

		_task_name_line_edit->setGeometry( x, y, width, height );

		_task_name_line_edit->setText( _selected_visible_task->task->task->name.c_str() );

		_task_name_line_edit->show();


	} else {
		_task_name_line_edit->hide();
	}
}

void TaskWeekEditor::paintEvent(QPaintEvent *)
{
	//TODO only call this on resize
	calcWindowResizeValues();

    //static const QPoint hourHand[3] = {
    //    QPoint(7, 8),
    //    QPoint(-7, 8),
    //    QPoint(0, -40)
    //};
    //static const QPoint minuteHand[3] = {
    //    QPoint(7, 8),
    //    QPoint(-7, 8),
    //    QPoint(0, -70)
    //};

    QColor grid_color(0, 0, 0);
	QColor grid_bg_color(255, 255, 255);
	QColor selected_task_color(0, 0, 127);
	QColor task_color(127, 127, 127);
    
	QColor task_background_colors[] = {
		QColor(247, 247, 247), 
		QColor(255, 255, 255) 
	};

	QColor selected_task_background_color( 255, 240, 240 );

    //int side = qMin(width(), height());
    //QTime time = QTime::currentTime();

    QPainter painter(this);
    //painter.setRenderHint(QPainter::Antialiasing);
    //painter.translate(width() / 2, height() / 2);
    //painter.scale(side / 200.0, side / 200.0);

    painter.setPen(grid_color);
    painter.setBrush(Qt::NoBrush);

	//draw ticks (and times)
	const int seconds_per_tick = getSecondsPerPlotTick();
	const int tick_x_start = _margin;
	const int tick_x_stop = _margin + _time_column_width;
	//int tick_background_count = 0;
	for( int tick_val = round_up(_day_start_time, seconds_per_tick); tick_val < _day_end_time && seconds_per_tick > 0; tick_val += seconds_per_tick ) {
		int tick_y = getYFromTime(tick_val);
		int next_tick_y = std::min( getYFromTime(tick_val + seconds_per_tick ), _grid_height );

		//painter.setPen(Qt::NoPen);
		//painter.setBrush( tick_background_colors[ tick_background_count % 2 ] );
		//tick_background_count++;
		//painter.drawRect( tick_x_start, tick_y, _grid_width, next_tick_y - tick_y );

		//painter.setPen(grid_color);
		painter.drawLine( tick_x_start, tick_y, tick_x_stop, tick_y );

		QTime q_time = QTime(0,0).addSecs(tick_val);
		assert( q_time.isValid() );
		QString label = q_time.toString("HH:mm");
		painter.drawText( tick_x_start + 10, tick_y, _time_column_width - 20, 40, Qt::AlignRight, label );
	}
	painter.setBrush(Qt::NoBrush);

	painter.drawRect(_margin, _margin + _top_heading_size, _grid_width, _grid_height);

	//draw tasks (and days)
	int task_i = 0;
	const int num_tasks = _visible_tasks.size();
	const int num_days = 7;
	for( int day_i = 0; day_i < NUM_DAYS; day_i++ ) {
		const int day_column_start_x = _margin + _time_column_width + day_i * (_day_column_width);

		//draw date text
		{
			QDateTime date_time;
			date_time.setTime_t(_week_start_time);
			date_time = date_time.addDays(day_i);
			painter.drawText( day_column_start_x, _top_heading_size, _day_column_width, _margin, Qt::AlignHCenter, date_time.date().toString() );
		}

		//draw tasks
		bool continued_task = false;
		while(true) {
			if( task_i >= num_tasks )
				break;
			const VisibleTask *visible_task = &_visible_tasks[task_i];
			const VisibleTask *next_visible_task = ( task_i + 1 >= num_tasks ? NULL : &_visible_tasks[task_i + 1] );
			if( visible_task->day_of_week > day_i )
				break;
			if( visible_task->day_of_week == day_i ) {
				int start_y = visible_task->start_y;
				int stop_y;

				//if( continued_task == true && task_i > 0 ) {
				//	int continued_task_start_y = 0;
				//}

				//TODO precalc this
				if( !next_visible_task || next_visible_task->day_of_week > day_i /*|| next_visible_task->start_y >= SECONDS_PER_DAY ##this looked buggy*/ ) {
					stop_y = _grid_height + _margin + _top_heading_size;
					continued_task = true;
				} else {
					stop_y = next_visible_task->start_y;
					continued_task = false;
				}

				const int task_height = stop_y - start_y;

				if( visible_task->task == _selected_task )
					painter.setBrush( selected_task_background_color );
				else {
					painter.setBrush( task_background_colors[ visible_task->task_i % 2] );
				}
				painter.drawRect( day_column_start_x, start_y, _day_column_width, stop_y - start_y );

				//draw start of task
				painter.drawLine( day_column_start_x, start_y, day_column_start_x + _day_column_width, start_y );

				//display start time of task
				if( _selected_boundary_task == visible_task->task && continued_task == false ) {
					//painter.setPen( _selected_boundary_task == visible_task->task ? selected_task_color : task_color );
					painter.drawText( day_column_start_x, start_y, _day_column_width, 20, Qt::AlignHCenter, formatTaskStartTime( visible_task->task->time ) );
				}

				//painter.setPen( grid_color );

				//display task label
				if( task_height > 40 ) {
					QString task_label = visible_task->task->task->name.c_str();
					task_label += "\n" + seconds_to_hours( visible_task->task_seconds );
					painter.drawText( day_column_start_x + 5, start_y, _day_column_width - 10, task_height, Qt::AlignCenter, task_label );
				}

				if( task_i == num_tasks - 1 ) {
					//draw end of task
					painter.drawLine( day_column_start_x, stop_y, day_column_start_x + _day_column_width, stop_y );
				}
				//draw text
				//int pixels_free = next_x - x;
				//int text_center_x = x + pixels_free / 2;

				//if( task_height > 30 ) {
				//	wxString label_text( visible_task->task->label );
				//	wxString str = getShortedString( dc, label_text, _day_column_width ) + "\n" + wxTimeSpan(0, 0, visible_task->task_seconds).Format("%H hours, %M min");
				//	if( visible_task->continued_task )
				//		str += "\n(continued)";
				//	dc.DrawLabel( str, wxRect(day_column_start_x, start_y, _day_column_width, task_height), wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL );
				//}
			}
			task_i++;
		}

		//draw start of day line (left line)
		painter.drawLine( day_column_start_x, _margin + _top_heading_size, day_column_start_x, _margin + _top_heading_size + _grid_height );
	}

	painter.save();
}

void TaskWeekEditor::mouseDoubleClickEvent( QMouseEvent *event )
{
	if( event->buttons() & Qt::LeftButton ) {
		_selected_visible_task = getVisibleTaskByPos( event->x(), event->y() );
		_selected_task = ( _selected_visible_task ? _selected_visible_task->task : NULL );

		if( _selected_task ) {
			LoggedTaskEditorDialog dl( _selected_task, _tasks, _user_settings, this);
			dl.exec();
		}
	}
}

void TaskWeekEditor::mousePressEvent( QMouseEvent *event )
{
	if( event->buttons() & Qt::LeftButton ) {
		LoggedTask *logged_task = getTaskByStartPos( event->x(), event->y() );
		if( logged_task ) {
			_selected_boundary_task = logged_task;
			_selected_boundary_task_initial_value = logged_task->time;
			_selected_task = NULL;
		} else {
			_selected_boundary_task = NULL;
			_selected_task = getTaskByPos( event->x(), event->y() );
			_selected_visible_task = NULL;
			setTaskEditorPosition();
		}
		update();
	}
}

void TaskWeekEditor::mouseReleaseEvent( QMouseEvent *event )
{
	if( _selected_boundary_task ) {
		_undo_actions.push( UndoAction( _selected_boundary_task, _selected_boundary_task_initial_value ) );
		_selected_boundary_task = NULL;
		//QToolTip::hideText();
		update();
		_tasks->save();
	}
}

void TaskWeekEditor::mouseMoveEvent( QMouseEvent *event )
{
	LoggedTask *logged_task = getTaskByStartPos( event->x(), event->y() );
    if (event->buttons() & Qt::LeftButton) {
        if( logged_task && _selected_boundary_task == NULL ) {
			_selected_boundary_task = logged_task;
		} else if( _selected_boundary_task ) {
			const int padding_secs = (_day_end_time - _day_start_time) / ( _grid_height / 4 );
			setTaskTimestamp(_selected_boundary_task, mousePosToTimestamp( event->x(), event->y() ), padding_secs );
			//QToolTip::showText( event->globalPos(), _selected_boundary_task->formatTaskStartTime() );
		}
    } else {
		//display resize cursor if task start is selected
		if( logged_task ) {
			setCursor( QCursor( Qt::SizeVerCursor ) );
			//QToolTip::showText( event->globalPos(), task->formatTaskStartTime() );
		} else {
			setCursor( QCursor( Qt::ArrowCursor ) );
			//QToolTip::hideText();
		}
		_selected_boundary_task = NULL;
	}
	event->accept();
}

void TaskWeekEditor::wheelEvent( QWheelEvent *event )
{
	int d = event->delta();
	event->x();
	event->y();

	const int wheel_rotation = -event->delta();
	const int time_diff = _day_end_time - _day_start_time;
	const int zoom_amount = std::max(time_diff * 0.001, 5.0);

	float zoom_percentage = static_cast<float>( event->y() - _margin - _top_heading_size ) / _grid_height;
	if( zoom_percentage < 0.f )
		zoom_percentage = 0.f;
	if( zoom_percentage > 1.f )
		zoom_percentage = 1.f;

	int x = zoom_amount * wheel_rotation;
	if( x <= -time_diff )
		return;

	_day_start_time -= x * zoom_percentage;
	_day_end_time += x * (1 - zoom_percentage);

	if( _day_start_time < 0 )
		_day_start_time = 0;

	if( _day_end_time >= SECONDS_PER_DAY ) 
		_day_end_time = SECONDS_PER_DAY - 1;

	if( _day_start_time < _day_start_time ) {
		_day_start_time = 0;
		_day_end_time = SECONDS_PER_DAY - 1;
	}

	calcVisibleTasks();
	event->accept();
	update();
}

//slots

void TaskWeekEditor::handleButton()
{
    // change the text
    setWindowTitle("button pushed");
	QObject *signal_sender = sender();
	if( signal_sender == _prev_week_button ) {
		setWeek( _week_start_time - 2*SECONDS_PER_DAY );
	} else if( signal_sender == _next_week_button ) {
		setWeek( _week_start_time + 8*SECONDS_PER_DAY );
	} else if( signal_sender == _this_week_button ) {
		setWeek( time(NULL) );
	}
	calcVisibleTasks();
	update();
}

void TaskWeekEditor::undo()
{
	if( _undo_actions.size() > 0 ) {
		_undo_actions.top().apply_undo();
		_undo_actions.pop();
		_tasks->save();
		calcVisibleTasks();
		update();
	}
}

UndoAction::UndoAction(LoggedTask *logged_task, time_t original_start_time ) :
		_logged_task( logged_task ),
		_original_start_time( original_start_time )
{}

void UndoAction::apply_undo()
{
	_logged_task->time = _original_start_time;
}
