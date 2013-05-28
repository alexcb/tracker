#ifndef TIMESERIESPANE_H_SDFNLX
#define TIMESERIESPANE_H_SDFNLX

#include "wx/wx.h"

#include <string>
#include <vector>

class TimeSeriesSpan
{
public:
	TimeSeriesSpan( time_t minutes_start, const char *label, void *data = NULL ) : time_start( minutes_start ), label( label ), data( data ) {}
	std::string label;
	time_t time_start;
	void *data;
};

class DisplayedTimeRange
{
public:
	//0 = sun, 1 = mon, ... 6 = sat
	int day_of_week;
	int start_y;
	int stop_y;
	bool continued_task;

	//1 - spring forward, 0 - no, -1 fall back
	int daylight_saving;
	TimeSeriesSpan *time_item;
	int task_seconds;
	bool in_progress;
};

typedef void (*timeChangedCallback)(void *data, time_t time, void *time_data);

class TimeSeriesPane : public wxPanel
{
    
public:
    TimeSeriesPane(wxFrame* parent);
    
    void paintEvent(wxPaintEvent & evt);
    void paintNow();
    
    void render(wxDC& dc);
    
	bool updateTimeEntry(void *data, const wxString& label);
	void addTimeEntry(time_t time, const wxString& label, void *data);
	void setWeek(time_t utc_time); // { _start_time = start; _end_time = end; calculateDisplayedTimes(); }

	//TODO should really just make this a virtual function
	//inline void setCallback( timeChangedCallback callback, void *callback_data ) { _on_change_callback = callback; _callback_data = callback_data; }

	virtual void taskStartTimeChanged( time_t new_time, void *data ) {};
	virtual void taskSelected( void *data ) {};
	virtual void taskDeselected() {};

	void mouseMoved(wxMouseEvent& event);
	void mouseDown(wxMouseEvent& event);
	void mouseReleased(wxMouseEvent& event);
	void mouseWheel(wxMouseEvent& event);

	void windowSized(wxSizeEvent& event);

    // some useful events
    /*
     
     
     void mouseWheelMoved(wxMouseEvent& event);
     
     void rightClick(wxMouseEvent& event);
     void mouseLeftWindow(wxMouseEvent& event);
     void keyPressed(wxKeyEvent& event);
     void keyReleased(wxKeyEvent& event);
     */
    
    DECLARE_EVENT_TABLE()

private:
	std::vector< TimeSeriesSpan > _data;
	std::vector< DisplayedTimeRange > _displayed_times;

	int _margin;
	time_t _start_time, _end_time;
	time_t _day_start_time, _day_end_time;

	//set when changing a time
	TimeSeriesSpan *_selected_time_entry;
	int _mouse_drag_y_start;

	//set when selecting a task (to change the name)
	TimeSeriesSpan *_selected_time_entry_label;

	void calculateDisplayedTimes();

	static inline bool taskVisibleInTimeSpan(time_t task_start_utc, time_t task_end_utc, time_t time_start_utc, time_t time_end_utc)
	{
		return time_start_utc <= task_end_utc && task_start_utc <= time_end_utc;
	}

	//variables related to widget placement (updated by calculateDisplayedTimes)
	int _grid_width;
	int _grid_height;
	int _grid_start_x;
	int _grid_start_y;
	int _day_column_width;

	const static int time_column_width;

	//returns the time entry corresponding to the select start time horizontal bar
	TimeSeriesSpan* getTaskStartByLocation( int x, int y );

	//return the time entry corresponding to the selected label
	TimeSeriesSpan* getTaskByLocation( int x, int y );

	time_t windowPosToTime( int x, int y ) const;

	inline int getDayOfWeekColumn( int x ) const { return (x - _grid_start_x - time_column_width) / _day_column_width; }

	timeChangedCallback  _on_change_callback;
	void *_callback_data;
};

#endif //TIMESERIESPANE_H_SDFNLX