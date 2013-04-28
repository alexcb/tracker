#include "viewer.h"

#include "wx/wx.h"
#include "wx/sizer.h"
 
#include "logged_tasks_time_series_pane.h"
#include "tracker_file_reader.h"

#include <limits>
 
IMPLEMENT_APP(ViewerApp)
 
bool ViewerApp::OnInit()
{
	//read in tasks
	_tasks = getTasks( getTrackerTaskFile().c_str() );

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    frame = new wxFrame((wxFrame *)NULL, -1,  wxT("Hello wxDC"), wxPoint(50,50), wxSize(800,600));
	
    time_series_pane = new LoggedTasksTimeSeriesPane( (wxFrame*) frame, this );

	//random data for jan 1, 1970 (from 00:00 - 23:59)
	//for( int i = 0; i < 24*60*60; i += 1 + rand() % (60*60) )
		//drawPane->addTimeEntry( i, "tsst", (void*) i );
	
	time_t time_min = std::numeric_limits<time_t>::max(), time_max = std::numeric_limits<time_t>::min();
	for( int i = 0; i < _tasks.size(); i++ ) {
		time_min = std::min( time_min, _tasks[i].time);
		time_max = std::max( time_max, _tasks[i].time);
		time_series_pane->addTimeEntry( _tasks[i].time, _tasks[i].name, (void*) i );
	}

	time_series_pane->setRange( time_min, time_max );

    sizer->Add(time_series_pane, 1, wxEXPAND);
	
	_task_label = new wxTextCtrl( frame, wxID_ANY );
	_task_label->Bind( wxEVT_COMMAND_TEXT_UPDATED, &ViewerApp::onTaskLabelChange, this );
	_selected_task = false;
	_task_label->Disable();
	sizer->Add(_task_label);

    frame->SetSizer(sizer);
    frame->SetAutoLayout(true);
	
    frame->Show();
    return true;
} 

void ViewerApp::onTaskLabelChange(wxCommandEvent& event)
{
	if( _selected_task ) {
		if( time_series_pane->updateTimeEntry( _selected_task_data, _task_label->GetValue() ) )
			time_series_pane->Refresh();
	}
}

void ViewerApp::timeRangeChanged(time_t time, void *time_data)
{
	int i = (int) time_data;
	assert( 0 <= i && i < _tasks.size() );
	_tasks[i].time = time;
}

void ViewerApp::selectTask(void *task_data)
{
	int i = (int) task_data;
	assert( 0 <= i && i < _tasks.size() );
	_selected_task = true;
	_selected_task_data = task_data;
	_task_label->Enable();
	_task_label->ChangeValue( _tasks[i].name.c_str() );
}

void ViewerApp::deselectTask()
{
	_selected_task = false;
	_task_label->ChangeValue( "" );
	_task_label->Disable();
}