#include "logged_tasks_time_series_pane.h"
#include "viewer.h"

void LoggedTasksTimeSeriesPane::taskStartTimeChanged( time_t new_time, void *data )
{
	_viewer_app->timeRangeChanged( new_time, data );
}

void LoggedTasksTimeSeriesPane::taskSelected( void *data )
{
	_viewer_app->selectTask( data );
}

void LoggedTasksTimeSeriesPane::taskDeselected()
{
	_viewer_app->deselectTask();
}
