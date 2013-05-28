#include "time_series_pane.h"

#include <wx/cursor.h>
#include <wx/gdicmn.h>
#include <wx/dcbuffer.h>

#include <sstream>
#include <iomanip>

#include "time_utils.h"

const int SECONDS_PER_DAY = 60 * 60 * 24;
const int DAYS_PER_WEEK = 7;
const int TimeSeriesPane::time_column_width = 100;

BEGIN_EVENT_TABLE(TimeSeriesPane, wxPanel)
	EVT_MOTION(TimeSeriesPane::mouseMoved)
	EVT_LEFT_DOWN(TimeSeriesPane::mouseDown)
	EVT_LEFT_UP(TimeSeriesPane::mouseReleased)
	EVT_MOUSEWHEEL(TimeSeriesPane::mouseWheel)
// some useful events
/*
 
 EVT_RIGHT_DOWN(TimeSeriesPane::rightClick)
 EVT_LEAVE_WINDOW(TimeSeriesPane::mouseLeftWindow)
 EVT_KEY_DOWN(TimeSeriesPane::keyPressed)
 EVT_KEY_UP(TimeSeriesPane::keyReleased)
 EVT_MOUSEWHEEL(TimeSeriesPane::mouseWheelMoved)
 */
 
// catch paint events
EVT_PAINT(TimeSeriesPane::paintEvent)

EVT_SIZE(TimeSeriesPane::windowSized)
 
END_EVENT_TABLE()
 
 
// some useful events
/*
 void TimeSeriesPane::mouseWheelMoved(wxMouseEvent& event) {}
  void TimeSeriesPane::rightClick(wxMouseEvent& event) {}
 void TimeSeriesPane::mouseLeftWindow(wxMouseEvent& event) {}
 void TimeSeriesPane::keyPressed(wxKeyEvent& event) {}
 void TimeSeriesPane::keyReleased(wxKeyEvent& event) {}
 */

TimeSeriesPane::TimeSeriesPane(wxFrame* parent) :
	wxPanel(parent),
	_margin( 30 ),
	_start_time( 0 ),
	_end_time( 100 ),
	_day_start_time( 0 ), 
	_day_end_time( 60*60*24 - 1 ),
	_selected_time_entry( NULL ),
	_on_change_callback( NULL ),
	_selected_time_entry_label( NULL )
{
	//setWeek(time(NULL));
	setWeek(1362937827);

	//spring forward
	OutputDebugStringA("spring forward\n");
	for( int hour = 0; hour < 4; hour += 1 ) {
		for( int sec = 0; sec < 60; sec += 30 ) {
			time_t ticks = dateTimeFromLocal(2013, 3, 10, hour, sec, 0, false).GetTicks();
			std::ostringstream oss;
			oss << hour << ":" << std::setfill('0') << std::setw(2) << sec << " -> " << ticks << std::endl;
			OutputDebugStringA( oss.str().c_str() );
		}
	}


	//spring forward
	OutputDebugStringA("fall back\n");
	for( int hour = 0; hour < 4; hour += 1 ) {
		for( int sec = 0; sec < 60; sec += 30 ) {
			time_t ticks = dateTimeFromLocal(2013, 11, 3, hour, sec, 0, true).GetTicks();
			std::ostringstream oss;
			oss << hour << ":" << std::setfill('0') << std::setw(2) << sec << " -> " << ticks << std::endl;
			OutputDebugStringA( oss.str().c_str() );
		}
	}

	int done = 123;
}

void TimeSeriesPane::setWeek(time_t utc_time)
{
	//convert utc_time into localtime, then figure out the time of sunday 00:00, then convert back to utc
	wxDateTime date_time(utc_time);
	date_time.SetToPrevWeekDay( wxDateTime::WeekDay::Sun );
	date_time.ResetTime();
	//epoch value (utc) which is num of secs until midnight sunday localtime
	_start_time = date_time.GetTicks();
	_end_time = _start_time + 60*60*24*7;
	calculateDisplayedTimes();
}

time_t TimeSeriesPane::windowPosToTime( int x, int y ) const
{
	const int selected_day_of_week = getDayOfWeekColumn(x);
	if( selected_day_of_week < 0 || selected_day_of_week > 6 ) {
		return -1;
	}
	const int time_span = _day_end_time - _day_start_time;
	y -= _grid_start_y;
	float start = static_cast<float>(y) / _grid_height;
	time_t time = start * time_span + _day_start_time;

	wxDateTime sunday_midnight(_start_time);
	wxDateTime::Tm day_tm = sunday_midnight.GetTm();

	time_t hour = time / (60*60);
	time_t min = (time % (60*60)) / 60;
	time_t sec = (time % (60));

	return dateTimeFromLocal(day_tm.year, day_tm.mon + 1, day_tm.mday, hour, min, sec, true).Add(wxDateSpan(0,0,0,selected_day_of_week)).GetTicks();
}

void TimeSeriesPane::mouseMoved(wxMouseEvent& event)
{
	int x = event.GetX();
	int y = event.GetY();

	if( event.Dragging() && _selected_time_entry ) {
		int new_time = windowPosToTime( x, y );

		//don't let time changes pass neighbouring times
		time_t prev_time = ( _selected_time_entry == &_data[0] ? 0 : (_selected_time_entry-1)->time_start );
		time_t next_time = ( _selected_time_entry == &_data.back() ? time(NULL) : (_selected_time_entry+1)->time_start );
		
		if( prev_time < new_time && new_time < next_time )
			_selected_time_entry->time_start = new_time;

		//TODO only change the surrounding tasks
		calculateDisplayedTimes();
		Refresh();
	} else if( event.Dragging() && !_selected_time_entry ) {
		_selected_time_entry = NULL;
		const int pixels_dragged = event.GetY() - _mouse_drag_y_start;

		const int time_span = _end_time - _start_time;
		const float percentage_dragged = static_cast<float>(pixels_dragged) / _grid_height;
		const time_t seconds_dragged = percentage_dragged * time_span;

		//int new_start_time = windowXToTime( time_dragged );
		//int time_diff = pixels_dragged > 0 ? -1 : pixels_dragged < 0 ? 1 : 0;
		time_t seconds_to_drag = std::min( seconds_dragged, _start_time );
		_start_time -= seconds_to_drag;
		_end_time -= seconds_to_drag;

		calculateDisplayedTimes();
		Refresh();

		_mouse_drag_y_start = event.GetY();
	} else {
		_selected_time_entry = NULL;
	}

	const TimeSeriesSpan *time_entry = getTaskStartByLocation( x, y );
	if( time_entry ) {
		SetCursor(wxCURSOR_SIZENS);
		wxDateTime task_start_time(time_entry->time_start);
		SetToolTip( task_start_time.FormatTime() );
	} else {
		SetCursor(wxCURSOR_ARROW);
		const TimeSeriesSpan *task_span = getTaskByLocation( x, y );
		if( task_span ) {
			SetToolTip( task_span->label );
		} else {
			UnsetToolTip();
		}
	}
}

void TimeSeriesPane::mouseDown(wxMouseEvent& event)
{
	_selected_time_entry = getTaskStartByLocation( event.GetX(), event.GetY() );
	if( _selected_time_entry ) {
		_selected_time_entry_label = NULL;
		taskDeselected();
	} else {
		//try selecting a task (inside the rectangle)
		_selected_time_entry_label = getTaskByLocation( event.GetX(), event.GetY() );
		if( _selected_time_entry_label )
			taskSelected( _selected_time_entry_label->data );
		else
			taskDeselected();
	}
	_mouse_drag_y_start = event.GetY();
	//calculateDisplayedTimes();
	Refresh();
}

void TimeSeriesPane::mouseReleased(wxMouseEvent& event)
{
	if( _selected_time_entry )
		taskStartTimeChanged( _selected_time_entry->time_start, _selected_time_entry->data );
	_selected_time_entry = NULL;
	calculateDisplayedTimes();
	Refresh();
}

void TimeSeriesPane::mouseWheel(wxMouseEvent& event)
{
	const int wheel_rotation = -event.GetWheelRotation();
	const int time_diff = _day_end_time - _day_start_time;
	const int zoom_amount = std::max(time_diff * 0.001, 5.0);

	const int height = GetSize().GetHeight();
	const int border_height = height - _margin * 2;
	const float zoom_percentage = static_cast<float>( event.GetY() - _margin ) / border_height;
	assert( 0.f <= zoom_percentage && zoom_percentage <= 1.f );

	int x = zoom_amount * wheel_rotation;
	if( x <= -time_diff )
		return;

	_day_start_time -= x * zoom_percentage;
	_day_end_time += x * (1 - zoom_percentage);

	if( _day_start_time < 0 )
		_day_start_time = 0;

	if( _day_end_time >= 60*60*24 ) 
		_day_end_time = 60*60*24 - 1;

	if( _end_time < _start_time ) {
		_day_start_time = 0;
		_day_end_time = 60*60*24 - 1;
	}

	assert( _start_time < _end_time );

	calculateDisplayedTimes();
	Refresh();
}

TimeSeriesSpan* TimeSeriesPane::getTaskStartByLocation( int x, int y )
{
	TimeSeriesSpan *found = NULL;
	const int selection_sensitivity = 2;
	const int num_items = _displayed_times.size();
	const int selected_day_of_week = getDayOfWeekColumn(x);
	for( int i = 0; i < num_items && found == NULL; i++ ) {
		if( _displayed_times[i].start_y - selection_sensitivity < y && y < _displayed_times[i].start_y + selection_sensitivity && \
			_displayed_times[i].day_of_week == selected_day_of_week	)
		{
			found = _displayed_times[i].time_item;
		}
	}
	return found;
}

TimeSeriesSpan* TimeSeriesPane::getTaskByLocation( int x, int y )
{
	const int width = GetSize().GetWidth();
	const int border_end_x = width - _margin;

	TimeSeriesSpan *found = NULL;
	const int num_items = _displayed_times.size();
	const int selected_day_of_week = (x - _grid_start_x - time_column_width) / _day_column_width;
	for( int i = 0; i < num_items && found == NULL; i++ ) {
		if( _displayed_times[i].day_of_week == selected_day_of_week ) {
			int end_y = ( i + 1 < num_items ? _displayed_times[i+1].start_y : _grid_height + _grid_start_y );
			if( _displayed_times[i].start_y < y && y < end_y ) {
				found = _displayed_times[i].time_item;
			}
		}
	}
	return found;
}

void TimeSeriesPane::calculateDisplayedTimes()
{
	_displayed_times.clear();

	const int height = GetSize().GetHeight();
	const int width = GetSize().GetWidth();

	_grid_width = width - 2 * _margin;
	_grid_height = height - 2 * _margin;
	_grid_start_x = _margin;
	_grid_start_y = _margin;
	_day_column_width = (_grid_width - time_column_width) / DAYS_PER_WEEK;
	
	
	
	//const int time_span = _end_time - _start_time;
	//assert( time_span > 0 );

	wxDateTime sunday_midnight(_start_time);
	wxDateTime::Tm day_tm = sunday_midnight.GetTm();
	time_t sunday_ticks = sunday_midnight.GetTicks();
	
	assert( 0 <= _day_start_time && _day_start_time < _day_end_time && _day_end_time < 24*60*60 );
	time_t start_hour = _day_start_time / (60*60);
	time_t start_min = (_day_start_time % (60*60)) / 60;
	time_t start_sec = (_day_start_time % (60));

	time_t end_hour = _day_end_time / (60*60);
	time_t end_min = (_day_end_time % (60*60)) / 60;
	time_t end_sec = (_day_end_time % (60));

	const int num_items = _data.size();
	for( int i = 0; i < num_items; i++ ) {

		//global (epoch) start/end times
		time_t task_start_time = _data[i].time_start;
		time_t task_end_time = ( i + 1 < num_items ? _data[i+1].time_start : time(NULL) );

		if( taskVisibleInTimeSpan( task_start_time, task_end_time, _start_time, _end_time ) ) {

			for( int day_of_week = 0; day_of_week < 7; day_of_week++ ) {

				//TODO this likely wont work for day light saving days
				time_t display_start = dateTimeFromLocal(day_tm.year, day_tm.mon + 1, day_tm.mday, start_hour, start_min, start_sec, true).Add(wxDateSpan(0,0,0,day_of_week)).GetTicks();
				time_t display_end = dateTimeFromLocal(day_tm.year, day_tm.mon + 1, day_tm.mday, end_hour, end_min, end_sec, true).Add(wxDateSpan(0,0,0,day_of_week)).GetTicks();

				if( taskVisibleInTimeSpan( task_start_time, task_end_time, display_start, display_end ) ) {
			
					DisplayedTimeRange time_range;
					time_range.time_item = &_data[i];
					//if( time_range.time_item->label == "interview prep" ) {
					//	int breaker = 1231;
					//	OutputDebugStringA("seen it\n");
					//}
					//TODO figure something out for time changes
					time_range.daylight_saving = 0;
					time_range.day_of_week = day_of_week;

					if( task_start_time < display_start ) {
						//start time of task is before the day being displayed
						time_range.start_y = _margin;
						time_range.continued_task = true;
					} else {
						//task start time is being displayed
						const time_t secs_since_midnight_local = getSecondsSinceLocalTimeMidnight(task_start_time);
						const float start_pct = static_cast<float>(secs_since_midnight_local - _day_start_time) / ( _day_end_time - _day_start_time );
						time_range.start_y = std::max( static_cast<int>(start_pct * _grid_height), 0 ) + _margin;
						time_range.continued_task = false;
					}

					time_range.task_seconds = ( i + 1 >= num_items ? time(NULL) : _data[i+1].time_start ) - _data[i].time_start;
					time_range.in_progress = i == num_items - 1;
					_displayed_times.push_back( time_range );
					break;
				}
			}
		}
	}

	//calculate the stop_y for each displayed task
	int num_displayed_items = _displayed_times.size();
	for( int i = 0; i < num_displayed_items; i++ ) {
		if( i + 1 < num_displayed_items ) {
			//next item exists

			while( _displayed_times[i].day_of_week < _displayed_times[i+1].day_of_week ) {
				DisplayedTimeRange tmp = _displayed_times[i];
				//set current span to end of day
				_displayed_times[i].stop_y = _grid_start_y + _grid_height;
				//and insert a new item for the next day
				i++;
				tmp.day_of_week++;
				tmp.start_y = _grid_start_y;
				tmp.continued_task = true;
				_displayed_times.insert( _displayed_times.begin() + i, tmp );
				num_displayed_items++;
			}

			_displayed_times[i].stop_y = _displayed_times[i+1].start_y;

		} else {
			//on the last item (either current item, or spans into a future date)

			const time_t secs_since_midnight_local = getSecondsSinceLocalTimeMidnight(_end_time);
			const float start_pct = static_cast<float>(secs_since_midnight_local - _day_start_time) / ( _day_end_time - _day_start_time );
			_displayed_times[i].stop_y = static_cast<int>(start_pct * _grid_height) + _margin;

			//TODO this will fail when the last task has spaned multiple days (need to do a similar loop here creating multiple loops here)
			//assert( _displayed_times[i].start_y < _displayed_times[i].stop_y );
		}
	}
}


void TimeSeriesPane::paintEvent(wxPaintEvent & evt)
{
    wxPaintDC dc(this);
	try {
    render(dc);
	} catch(...) {
		assert(0);
	}
}
 
void TimeSeriesPane::paintNow()
{
    wxClientDC dc(this);
	try {
    render(dc);
	} catch(...) {
		assert(0);
	}
}

bool TimeSeriesPane::updateTimeEntry(void *data, const wxString& label)
{
	//TODO do a sorted insert instead of assuming it'll just work on the end
	const int num_items = _data.size();
	bool found = false;
	for( int i = 0; i < num_items && found == false; i++ ) {
		if( _data[i].data == data ) {
			found = true;
			_data[i].label = label;
		}
	}
	return found;
}

void TimeSeriesPane::addTimeEntry(time_t time, const wxString& label, void *data)
{
	//TODO do a sorted insert instead of assuming it'll just work on the end
	if( _data.size() > 0 )
		assert( _data.back().time_start <= time );

	_data.push_back( TimeSeriesSpan( time, label.c_str(), data ) );
}

wxString getShortedString(wxDC& dc, wxString str, int max_width)
{
	long x, y;
	dc.GetTextExtent(str, &x, &y);
	bool shortened = false;
	wxString s = str;
	while( x > max_width && str.Length() > 0 ) {
		//remove one char at a time (except if first time, remove 3)
		str = str.substr( 0, str.Length() - 1 );
		s = str + "...";
		dc.GetTextExtent(s, &x, &y);
		shortened = true;
	}
	if( shortened && s.Length() == 3 )
		s = "";
	return s;

}

void TimeSeriesPane::render(wxDC& dc)
{
	dc.SetPen( wxPen( wxColor(0,0,0), 1 ) ); 

	//time column
	dc.DrawRectangle( _grid_start_x, _grid_start_y, time_column_width+1, _grid_height );

	//draw time ticks
	const int tick_spacing = 50;
	const int num_ticks = _grid_height / tick_spacing;
	const int day_seconds_displayed = _day_end_time - _day_start_time;
	if( num_ticks > 1 ) {

		int seconds_per_tick = day_seconds_displayed / num_ticks;

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


		const int tick_x_start = _grid_start_x + 1;
		const int tick_x_end = _grid_start_x + time_column_width;

		dc.SetPen( wxPen(wxColor(192,192,192), 1, wxPENSTYLE_SOLID) );
		for( int tick_val = round_up(_day_start_time, seconds_per_tick); tick_val < _day_end_time; tick_val += seconds_per_tick ) {
			float tick_pct = static_cast<float>(tick_val - _day_start_time) / day_seconds_displayed;
			int tick_y = tick_pct * _grid_height + _margin;
		
			if( tick_y > _grid_start_y ) {
				dc.DrawLine( tick_x_start, tick_y, tick_x_end, tick_y );
			}
			wxString time_str = wxDateTime(static_cast<time_t>(tick_val)).Format("%H:%M", wxDateTime::UTC);
			//TODO dont draw out of the margins
			dc.DrawLabel( time_str, wxRect(tick_x_start, tick_y, time_column_width, tick_spacing), wxALIGN_RIGHT ); //, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL );
		}
		dc.SetPen( wxPen(wxColor(0,0,0), 1, wxPENSTYLE_SOLID) );
	}

	//day columns
	int task_i = 0;
	const int num_tasks = _displayed_times.size();
	const int num_days = 7;
	for( int day_i = 0; day_i < num_days; day_i++ ) {
		int day_column_start_x = _grid_start_x + time_column_width + day_i * (_day_column_width);
		dc.DrawRectangle( day_column_start_x, _grid_start_y, _day_column_width+1, _grid_height );

		//draw day label
		wxString date_str = wxDateTime(static_cast<time_t>(_start_time + day_i*24*60*60)).Format("%a %b %d", wxDateTime::UTC);
		dc.DrawLabel( date_str, wxRect(day_column_start_x, 0, _day_column_width, _margin), wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL );

		//draw tasks
		bool continued_task = false;
		while(true) {
			if( task_i >= num_tasks )
				break;
			if( _displayed_times[task_i].day_of_week > day_i )
				break;
			if( _displayed_times[task_i].day_of_week == day_i ) {
				int start_y = _displayed_times[task_i].start_y;
				int stop_y;

				//if( continued_task == true && task_i > 0 ) {
				//	int continued_task_start_y = 0;
				//}

				//TODO precalc this
				if( task_i + 1 >= num_tasks || _displayed_times[task_i + 1].day_of_week > day_i || _displayed_times[task_i + 1].start_y >= SECONDS_PER_DAY ) {
					stop_y = _grid_height + _margin;
					continued_task = true;
				} else {
					stop_y = _displayed_times[task_i + 1].start_y;
					continued_task = false;
				}

				dc.DrawLine( day_column_start_x, start_y, day_column_start_x + _day_column_width, start_y );

				if( task_i == num_tasks - 1 ) {
					//draw end of task
					dc.DrawLine( day_column_start_x, stop_y, day_column_start_x + _day_column_width, stop_y );
				}
				//draw text
				//int pixels_free = next_x - x;
				//int text_center_x = x + pixels_free / 2;

				int task_height = stop_y - start_y;
				if( task_height > 30 ) {
					wxString label_text( _displayed_times[task_i].time_item->label );
					wxString str = getShortedString( dc, label_text, _day_column_width ) + "\n" + wxTimeSpan(0, 0, _displayed_times[task_i].task_seconds).Format("%H hours, %M min");
					if( _displayed_times[task_i].continued_task )
						str += "\n(continued)";
					dc.DrawLabel( str, wxRect(day_column_start_x, start_y, _day_column_width, task_height), wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL );
				}
			}
			task_i++;
		}
	}

	//draw labels


	//
	//const int time_span = _end_time - _start_time;
	//assert( time_span > 0 );

	//const int border_width = width - _margin * 2;
	//const int border_height = height;

	//// black line, 1 pixels thick
	//dc.SetPen( wxPen( wxColor(0,0,0), 1 ) ); 

	//// draw rectangle border
	//const int border_start_x = _margin;
	//const int border_end_x = _margin + border_width;
	//dc.DrawRectangle( _margin, _margin, border_width, border_height );

	//const int num_items = _displayed_times.size();
	//int y_bottom = _margin;
	//int y_top = _margin + border_height;
	//for( int i = 0; i < num_items; i++ ) {
	//	int x = _displayed_times[i].x;

	//	//draw draggable line if it is in the range of the widget
	//	if( border_start_x < x && x < border_end_x )
	//		dc.DrawLine( x, y_bottom, x, y_top );

	//	if( x < border_start_x )
	//		x = border_start_x;

	//	//get location of next task (or end of rectangle widget)
	//	int next_x = ( i + 1 < num_items ? _displayed_times[i+1].x : border_end_x );
	//	if( next_x > _margin + border_width )
	//		next_x = _margin + border_width;

	//	//draw selected background
	//	if( _selected_time_entry_label == _displayed_times[i].time_item ) {
	//		wxPen old_pen = dc.GetPen();
	//		dc.SetPen( wxPen( wxColor(255,1,0), 1 ) ); 
	//		dc.DrawRectangle( x+1, _margin+1, next_x - x - 1, border_height-2 );
	//		dc.SetPen( old_pen ); 
	//	}

	//	//draw text
	//	int pixels_free = next_x - x;
	//	int text_center_x = x + pixels_free / 2;

	//	wxString label_text( _displayed_times[i].time_item->label );
	//	wxString str = getShortedString( dc, label_text, pixels_free );

	//	dc.DrawLabel( str, wxRect(x, y_bottom, pixels_free, border_height/2), wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL );

	//	//draw task duration
	//	wxTimeSpan task_time_span( 0, 0, _displayed_times[i].task_seconds );

	//	std::ostringstream oss;
	//			
	//	if( _displayed_times[i].task_seconds > 60*60 )
	//		oss << "%H hours ";
	//	oss << "%M mins";

	//	if( _displayed_times[i].in_progress )
	//		oss << " (current task)";
	//	
	//	std::string format_time_string = oss.str();
	//	str = getShortedString( dc, task_time_span.Format( format_time_string.c_str() ), pixels_free );
	//	dc.DrawLabel( str, wxRect(x, y_bottom + border_height/2, pixels_free, border_height/2), wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL );
	//}
	//
	////draw time that's being changed above
	//if( _selected_time_entry != NULL ) {
	//	for( int i = 0; i < num_items; i++ ) {
	//		if( _displayed_times[i].time_item == _selected_time_entry ) {
	//			wxDateTime date_time( _selected_time_entry->time_start );
	//			wxString date_time_str = date_time.Format("%Y-%m-%d %H:%M:%S");
	//			
	//			int str_width, str_height;
	//			dc.GetTextExtent(date_time_str, &str_width, &str_height);

	//			int x = _displayed_times[i].x - str_width/2;
	//			dc.DrawLabel( date_time_str, wxRect(x, y_bottom - str_height, str_width, str_height), wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL );
	//		}
	//	}
	//}

	////draw ticks below

	//const int num_ticks = width / 100;
	//int min_step = 60;
	////if( time_span > 60*5 )
	////	min_step = 60*2;
	////if( time_span > 60*20 )
	////	min_step = 60*5;
	//if( time_span > 60*60*3 )
	//	min_step = 60 * 15;
	//if( time_span > 60*60*12 )
	//	min_step = 60 * 60;

	//time_t tick_interval = time_span / num_ticks / min_step * min_step;
	//time_t tick_val = _start_time;
	//if( tick_interval == 0 )
	//	tick_interval = 60;
	//if( tick_interval > 0 && tick_val % tick_interval > 0 ) {
	//	tick_val += tick_interval - tick_val % tick_interval;
	//}

	////TODO tick_val should line up on the localtime zone, not UTC

	//time_t last_day = -1;
	//time_t tick_day;
	//for( ; tick_interval > 0 && tick_val < _end_time; tick_val += tick_interval ) {
	//	float tick_percentage = (static_cast<float>(tick_val) - _start_time) / time_span;
	//	assert( 0.0 <= tick_percentage && tick_percentage <= 1.0 );
	//	int tick_x = _margin + tick_percentage * border_width;
	//	dc.DrawLine( tick_x, y_top, tick_x, y_top + 10 );
	//	
	//	//draw time label
	//	wxDateTime date_time( tick_val );
	//	wxString date_time_str = date_time.Format("%H:%M\n");

	//	//TODO make this work for the system timezone
	//	tick_day = wxDateTime(tick_val).GetDayOfYear();
	//	if( last_day != tick_day ) {
	//		date_time_str += date_time.Format("%Y-%m-%d");
	//		last_day = tick_day;
	//	}

	//	int str_width, str_height;
	//	dc.GetTextExtent(date_time_str, &str_width, &str_height);

	//	int text_x = tick_x - str_width/2;
	//	dc.DrawLabel( date_time_str, wxRect(text_x, y_top + 20, str_width, str_height), wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL );
	//}

}

void TimeSeriesPane::windowSized(wxSizeEvent& event)
{
	calculateDisplayedTimes();
	Refresh();
}