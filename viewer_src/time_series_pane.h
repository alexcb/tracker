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
	int x;
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
	inline void setRange(time_t start, time_t end) { _start_time = start; _end_time = end; calculateDisplayedTimes(); }

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

	//set when changing a time
	TimeSeriesSpan *_selected_time_entry;
	int _mouse_drag_x_start;

	//set when selecting a task (to change the name)
	TimeSeriesSpan *_selected_time_entry_label;

	void calculateDisplayedTimes();

	//returns the time entry corresponding to the select start time horizontal bar
	TimeSeriesSpan* getSelectedTimeEntry( int x );

	//return the time entry corresponding to the selected label
	TimeSeriesSpan* getSelectedTimeEntryLabel( int x );

	int windowXToTime( int x ) const;\

	timeChangedCallback  _on_change_callback;
	void *_callback_data;
};

#endif //TIMESERIESPANE_H_SDFNLX