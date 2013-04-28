#ifndef LOGGED_TASKS_TIME_SERIES_PANE_SFNXBLBZ
#define LOGGED_TASKS_TIME_SERIES_PANE_SFNXBLBZ

#include "time_series_pane.h"

class ViewerApp;

class LoggedTasksTimeSeriesPane : public TimeSeriesPane
{
public:
	LoggedTasksTimeSeriesPane(wxFrame* parent, ViewerApp *app) : TimeSeriesPane(parent), _viewer_app(app) {}
protected:
	virtual void taskStartTimeChanged( time_t new_time, void *data );
	virtual void taskSelected( void *data );
	virtual void taskDeselected();
private:
	ViewerApp *_viewer_app;
};

#endif //LOGGED_TASKS_TIME_SERIES_PANE_SFNXBLBZ