#include "time_series_pane.h"

#include <wx/cursor.h>
#include <wx/gdicmn.h>
#include <wx/dcbuffer.h>

#include <sstream>

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
	_selected_time_entry( NULL ),
	_on_change_callback( NULL ),
	_selected_time_entry_label( NULL )
{

}

int TimeSeriesPane::windowXToTime( int x ) const
{
	const int width = GetSize().GetWidth();
	const int border_width = width - _margin * 2;
	const int time_span = _end_time - _start_time;

	x -= _margin;
	float start = static_cast<float>(x) / border_width;
	int time = start * time_span + _start_time;
	return time;
}

void TimeSeriesPane::mouseMoved(wxMouseEvent& event)
{
	int x = event.GetX();
	int y = event.GetY();

	if( event.Dragging() && _selected_time_entry ) {
		int new_time = windowXToTime( x );

		//don't let time changes pass neighbouring times
		int prev_time = ( _selected_time_entry == &_data[0] ? 0 : (_selected_time_entry-1)->time_start );
		int next_time = ( _selected_time_entry == &_data.back() ? 9999999999 : (_selected_time_entry+1)->time_start );
		
		if( prev_time < new_time && new_time < next_time )
			_selected_time_entry->time_start = new_time;

		calculateDisplayedTimes();
		Refresh();
	} else if( event.Dragging() && !_selected_time_entry ) {
		_selected_time_entry = NULL;
		const int pixels_dragged = event.GetX() - _mouse_drag_x_start;

		const int width = GetSize().GetWidth();
		const int border_width = width - _margin * 2;
		const int time_span = _end_time - _start_time;

		const float percentage_dragged = static_cast<float>(pixels_dragged) / border_width;
		const time_t seconds_dragged = percentage_dragged * time_span;

		//int new_start_time = windowXToTime( time_dragged );
		//int time_diff = pixels_dragged > 0 ? -1 : pixels_dragged < 0 ? 1 : 0;
		time_t seconds_to_drag = std::min( seconds_dragged, _start_time );
		_start_time -= seconds_to_drag;
		_end_time -= seconds_to_drag;

		calculateDisplayedTimes();
		Refresh();

		_mouse_drag_x_start = event.GetX();
	} else {
		_selected_time_entry = NULL;
	}

	TimeSeriesSpan *time_entry = getSelectedTimeEntry( x );
	if( time_entry ) {
		SetCursor(wxCURSOR_SIZEWE);
	} else {
		SetCursor(wxCURSOR_ARROW);
	}
}

void TimeSeriesPane::mouseDown(wxMouseEvent& event)
{
	_selected_time_entry = getSelectedTimeEntry( event.GetX() );
	if( _selected_time_entry == NULL ) {
		_selected_time_entry_label = getSelectedTimeEntryLabel( event.GetX() );
		if( _selected_time_entry_label )
			taskSelected( _selected_time_entry_label->data );
		else
			taskDeselected();
	} else {
		_selected_time_entry_label = NULL;
		taskDeselected();
	}
	_mouse_drag_x_start = event.GetX();
	calculateDisplayedTimes();
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
	const int time_diff = _start_time - _end_time;
	const int zoom_amount = 10; //std::min( time_diff / 5, 5 );

	const int width = GetSize().GetWidth();
	const int border_width = width - _margin * 2;
	const float zoom_percentage = static_cast<float>( event.GetX() - _margin ) / border_width;
	assert( 0.f <= zoom_percentage && zoom_percentage <= 1.f );

	int x = zoom_amount * wheel_rotation;

	_start_time -= x * zoom_percentage;
	_end_time += x * (1 - zoom_percentage);

	if( _start_time < 0 )
		_start_time = 0;
	if( _end_time < _start_time )
		_end_time = _start_time + 60 * 5;

	assert( _start_time < _end_time );

	calculateDisplayedTimes();
	Refresh();
}

TimeSeriesSpan* TimeSeriesPane::getSelectedTimeEntry( int x )
{
	TimeSeriesSpan *found = NULL;
	const int num_items = _displayed_times.size();
	for( int i = 0; i < num_items && found == NULL; i++ ) {
		if( _displayed_times[i].x - 2 < x && x < _displayed_times[i].x + 2 )
			found = _displayed_times[i].time_item;
	}
	return found;
}

TimeSeriesSpan* TimeSeriesPane::getSelectedTimeEntryLabel( int x )
{
	const int width = GetSize().GetWidth();
	const int border_end_x = width - _margin;

	TimeSeriesSpan *found = NULL;
	const int num_items = _displayed_times.size();
	for( int i = 0; i < num_items && found == NULL; i++ ) {
		int end_time_x = ( i + 1 < num_items ? _displayed_times[i+1].x : border_end_x );
		if( _displayed_times[i].x < x && x < end_time_x )
			found = _displayed_times[i].time_item;
	}
	return found;
}

void TimeSeriesPane::calculateDisplayedTimes()
{
	_displayed_times.clear();
	
	const int width = GetSize().GetWidth();
	const int border_width = width - _margin * 2;
	const int time_span = _end_time - _start_time;
	assert( time_span > 0 );

	const int num_items = _data.size();
	for( int i = 0; i < num_items; i++ ) {
		float start = static_cast<float>(_data[i].time_start - _start_time) / time_span;
		float end = static_cast<float>( i + 1 < num_items ? _data[i+1].time_start : time(NULL) - _start_time) / time_span;

		if( start <= 1.f && end >= 0.f ) {
		//should be equivelant to: if( 0.f <= start && start <= 1.f || 0.f <= end && end <= 1.f || start < 0.f && end > 1.f ) {
			DisplayedTimeRange time_range;
			time_range.time_item = &_data[i];
			time_range.x = _margin + start * border_width;
			time_range.task_seconds = ( i + 1 >= num_items ? time(NULL) : _data[i+1].time_start ) - _data[i].time_start;
			time_range.in_progress = i == num_items - 1;
			_displayed_times.push_back( time_range );
		}
	}
}


/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */
 
void TimeSeriesPane::paintEvent(wxPaintEvent & evt)
{
    wxPaintDC dc(this);
	try {
    render(dc);
	} catch(...) {
		assert(0);
	}
}
 
/*
 * Alternatively, you can use a clientDC to paint on the panel
 * at any time. Using this generally does not free you from
 * catching paint events, since it is possible that e.g. the window
 * manager throws away your drawing when the window comes to the
 * background, and expects you will redraw it when the window comes
 * back (by sending a paint event).
 *
 * In most cases, this will not be needed at all; simply handling
 * paint events and calling Refresh() when a refresh is needed
 * will do the job.
 */
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

/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
void TimeSeriesPane::render(wxDC& dc)
{
	//double buffer the dc
	//wxBufferedDC dc( &raw_dc, GetSize() );

	//dc.Clear();

	calculateDisplayedTimes();

	int height = 50;
	int width = GetSize().GetWidth();
	
	const int time_span = _end_time - _start_time;
	assert( time_span > 0 );

	const int border_width = width - _margin * 2;
	const int border_height = height;

	// black line, 1 pixels thick
	dc.SetPen( wxPen( wxColor(0,0,0), 1 ) ); 

	// draw rectangle border
	const int border_start_x = _margin;
	const int border_end_x = _margin + border_width;
	dc.DrawRectangle( _margin, _margin, border_width, border_height );

	const int num_items = _displayed_times.size();
	int y_bottom = _margin;
	int y_top = _margin + border_height;
	for( int i = 0; i < num_items; i++ ) {
		int x = _displayed_times[i].x;

		//draw draggable line if it is in the range of the widget
		if( border_start_x < x && x < border_end_x )
			dc.DrawLine( x, y_bottom, x, y_top );

		if( x < border_start_x )
			x = border_start_x;

		//get location of next task (or end of rectangle widget)
		int next_x = ( i + 1 < num_items ? _displayed_times[i+1].x : border_end_x );
		if( next_x > _margin + border_width )
			next_x = _margin + border_width;

		//draw selected background
		if( _selected_time_entry_label == _displayed_times[i].time_item ) {
			wxPen old_pen = dc.GetPen();
			dc.SetPen( wxPen( wxColor(255,1,0), 1 ) ); 
			dc.DrawRectangle( x+1, _margin+1, next_x - x - 1, border_height-2 );
			dc.SetPen( old_pen ); 
		}

		//draw text
		int pixels_free = next_x - x;
		int text_center_x = x + pixels_free / 2;

		wxString label_text( _displayed_times[i].time_item->label );
		wxString str = getShortedString( dc, label_text, pixels_free );

		dc.DrawLabel( str, wxRect(x, y_bottom, pixels_free, border_height/2), wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL );

		//draw task duration
		wxTimeSpan task_time_span( 0, 0, _displayed_times[i].task_seconds );

		std::ostringstream oss;
				
		if( _displayed_times[i].task_seconds > 60*60 )
			oss << "%H hours ";
		oss << "%M mins";

		if( _displayed_times[i].in_progress )
			oss << " (current task)";
		
		std::string format_time_string = oss.str();
		str = getShortedString( dc, task_time_span.Format( format_time_string.c_str() ), pixels_free );
		dc.DrawLabel( str, wxRect(x, y_bottom + border_height/2, pixels_free, border_height/2), wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL );
	}
	
	//draw time that's being changed above
	if( _selected_time_entry != NULL ) {
		for( int i = 0; i < num_items; i++ ) {
			if( _displayed_times[i].time_item == _selected_time_entry ) {
				wxDateTime date_time( _selected_time_entry->time_start );
				wxString date_time_str = date_time.Format("%Y-%m-%d %H:%M:%S");
				
				int str_width, str_height;
				dc.GetTextExtent(date_time_str, &str_width, &str_height);

				int x = _displayed_times[i].x - str_width/2;
				dc.DrawLabel( date_time_str, wxRect(x, y_bottom - str_height, str_width, str_height), wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL );
			}
		}
	}

	//draw ticks below

	const int num_ticks = 10;
	int min_step = 60;
	//if( time_span > 60*5 )
	//	min_step = 60*2;
	//if( time_span > 60*20 )
	//	min_step = 60*5;
	if( time_span > 60*60*3 )
		min_step = 60 * 15;
	if( time_span > 60*60*12 )
		min_step = 60 * 60;

	time_t tick_interval = time_span / num_ticks / min_step * min_step;
	time_t tick_val = _start_time;
	if( tick_interval == 0 )
		tick_interval = 60;
	if( tick_interval > 0 && tick_val % tick_interval > 0 ) {
		tick_val += tick_interval - tick_val % tick_interval;
	}

	for( ; tick_interval > 0 && tick_val < _end_time; tick_val += tick_interval ) {
		float tick_percentage = (static_cast<float>(tick_val) - _start_time) / time_span;
		assert( 0.0 <= tick_percentage && tick_percentage <= 1.0 );
		int tick_x = _margin + tick_percentage * border_width;
		dc.DrawLine( tick_x, y_top, tick_x, y_top + 10 );
		
		//draw time label
		wxDateTime date_time( tick_val );
		wxString date_time_str = date_time.Format("%H:%M");

		int str_width, str_height;
		dc.GetTextExtent(date_time_str, &str_width, &str_height);

		int text_x = tick_x - str_width/2;
		dc.DrawLabel( date_time_str, wxRect(text_x, y_top + 13, str_width, str_height), wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL );
	}
}

void TimeSeriesPane::windowSized(wxSizeEvent& event)
{
	calculateDisplayedTimes();
	Refresh();
}