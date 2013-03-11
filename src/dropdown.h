#include <wx/wx.h>
#include <wx/popupwin.h>

#include <vector>
#include <string>

class wxScrollBar;
class wxTextCtrl;
class wxScrollBar;
class DropDownPopupWindow;

class TaskDropDown : public wxTextCtrl
{
 
    DropDownPopupWindow *_popup;
	int _max_height;
	bool _perform_autocomplete;

	//array of items to display
	wxArrayString items;

public:
    //TaskDropDown(wxFrame* parent, wxWindowID winid);

	TaskDropDown(wxFrame *parent,
               wxWindowID id,
               const wxString &value = wxEmptyString,
               const wxPoint &pos = wxDefaultPosition,
               const wxSize &size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString &name = wxTextCtrlNameStr);
	
	int item_height;

	//the index of the first item to display
	int displayed_page_first_item;

	//the number of items per page
	int displayed_page_num_items;

	//the currently selected item (index to intems array
	int selected_item;

	wxArrayString matching_items;
	
	void addItem( const char *item );

	const wxArrayString& getItems() const { return items; }
        
	void onText(wxCommandEvent& event);
	void onKeyDown(wxKeyEvent &event);
	void onFocus(wxFocusEvent &event);

    DECLARE_EVENT_TABLE()
};
