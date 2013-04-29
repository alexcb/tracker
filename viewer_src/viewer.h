#ifndef VIEWER_H_SDVNXLZ
#define VIEWER_H_SDVNXLZ

#include "wx/wx.h"
#include <vector>
#include "tracker_file_reader.h"

class wxFrame;
class LoggedTasksTimeSeriesPane;
 
class ViewerApp: public wxApp
{
    bool OnInit();
    
    wxFrame *frame;
    LoggedTasksTimeSeriesPane *time_series_pane;

public:
	void timeRangeChanged(time_t time, void *time_data);
	void selectTask(void *task_data);
	void deselectTask();

	void onTaskLabelChange(wxCommandEvent& event);

	void onSave(wxCommandEvent& event);
private:
	std::vector< Task > _tasks;
    wxTextCtrl *_task_label;
	bool _selected_task;
	void *_selected_task_data;

	wxMenuBar *_menu_bar;
	wxMenu *_file_menu;

	DECLARE_EVENT_TABLE()
};

#endif //VIEWER_H_SDVNXLZ