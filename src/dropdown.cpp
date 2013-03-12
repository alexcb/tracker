#include "dropdown.h"
#include <sstream>

class DropDownPopupWindow : public wxPopupWindow
{
	//wxArrayString _items;
	//int _displayed_page_num_items;
	//int _displayed_page_first_item;

	TaskDropDown *_parent_drop_down;
public:

	DropDownPopupWindow(wxWindow *parent, TaskDropDown *dropdown)
		: wxPopupWindow(parent)
		, _parent_drop_down( dropdown )
		, item_height( 25 )

	{
	}

	const int item_height;

	//void calcSize()
	//{
	//	const int num_items = std::min<int>( _parent_drop_down->displayed_page_num_items, _parent_drop_down->items.size() ); //TODO get this based on size
	//	const int width = GetSize().x;
	//	SetSize( width, num_items * item_height );
	//}

	void paintEvent(wxPaintEvent & evt)
	{
		wxPaintDC dc(this);

		//wxColor color_2( 220, 0, 220 );
		//wxBrush brush( color_2, wxBRUSHSTYLE_SOLID );
		//dc.SetBrush( brush );
		//dc.DrawRectangle( 0, 0, GetSize().x, GetSize().y );

		const int num_items = std::min<int>( _parent_drop_down->displayed_page_num_items, _parent_drop_down->matching_items.size() ); //TODO get this based on size
		const int width = GetSize().x;
		const int text_offset_y = 2;
		wxColor color_1( 200, 200, 200 );
		wxColor color_2( 220, 220, 220 );
		wxColor color_selected( 100, 100, 220 );
		wxBrush background_1( color_1, wxBRUSHSTYLE_SOLID );
		wxBrush background_2( color_2, wxBRUSHSTYLE_SOLID );
		wxBrush background_selected( color_selected, wxBRUSHSTYLE_SOLID );
		wxBrush *backgrounds[2] = { &background_1, &background_2 };
		wxPen no_pen( color_1, 0 );

		for( int i = 0; i < num_items; i++ ) {
			int j = _parent_drop_down->displayed_page_first_item + i;
			//assert( j < _items.size() );
			dc.SetPen( no_pen );
			if( _parent_drop_down->selected_item == j )
				dc.SetBrush( background_selected );
			else
				dc.SetBrush( *backgrounds[ j % 2 ] );
			dc.DrawRectangle( 0, item_height * i, width, item_height );
			dc.DrawText( _parent_drop_down->matching_items[j].c_str(), 0, item_height * i + text_offset_y );
		}
	}

	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(DropDownPopupWindow, wxPopupWindow)
	EVT_PAINT(DropDownPopupWindow::paintEvent)
END_EVENT_TABLE()







 
BEGIN_EVENT_TABLE(TaskDropDown, wxTextCtrl)
	EVT_KEY_DOWN(TaskDropDown::onKeyDown)
	EVT_TEXT(wxID_ANY, TaskDropDown::onText)
	EVT_KILL_FOCUS(TaskDropDown::onFocus)
END_EVENT_TABLE()


//_task_text_ctrl = new wxTextCtrl( this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize( 300, wxDefaultSize.GetY() ), wxTE_PROCESS_ENTER);

TaskDropDown::TaskDropDown(wxFrame *parent,
               wxWindowID id,
               const wxString &value,
               const wxPoint &pos,
               const wxSize &size,
               long style,
               const wxValidator& validator,
               const wxString &name) 
	: 
	wxTextCtrl(parent, id, value, pos, size, style, validator, name),
	_max_height(200),
	_perform_autocomplete( false )
{
	displayed_page_first_item = 0;
	displayed_page_num_items = 5;
	selected_item = 0;

	_popup = new DropDownPopupWindow(parent, this);
}

void TaskDropDown::onText(wxCommandEvent& event)
{
	//search for matching strings
	matching_items.Clear();
	selected_item = 0;
	displayed_page_first_item = 0;

	const wxString str = GetValue();
	const int str_len = str.Length();

	//first look for substrings
	int matches_i = 0;
	if( str_len > 0 ) {
		for( int i = 0; i < items.size(); i++ ) {
			int res = items[i].compare( 0, str_len, str );
			if( res == 0 ) {
				matching_items.Add( items[ i ] );
			}
		}
	}

	//auto complete the first item
#ifdef WIN32
//FIXME having troubles with linux support not setting the selection (which makes typing unusable)
	if( _perform_autocomplete && str_len > 0 && matching_items.size() > 0 ) {
		int matched_len = matching_items[ 0 ].Length();
		if( str_len < matched_len ) {
			ChangeValue( matching_items[ 0 ] );
			SetSelection(str_len, matched_len );
		}
	}
#endif
	//only do this once, unless the onKeyDown specifies another real user key (this prevents auto completing when backspacing)
	_perform_autocomplete = false;

	//now search for any matches (not just prefixes)
	if( str_len > 0 ) {
		for( int i = 0; i < items.size(); i++ ) {
			if( items[i].Contains(str) ) {
				if( matching_items.Index( items[i] ) == wxNOT_FOUND )
					matching_items.Add( items[i] );
			}
		}
	}

	wxPoint pt = GetParent()->ClientToScreen( GetPosition() );
	wxSize sz = GetSize();

	const int num_items = std::min<int>( displayed_page_num_items, matching_items.size() );
	const int height = num_items * _popup->item_height;
	
	_popup->SetSize( sz.x, height );
	_popup->Position( pt, wxSize( 0, sz.y ) );
	//_popup->calcSize();
	_popup->Refresh();
	_popup->Show();
}

void TaskDropDown::onKeyDown(wxKeyEvent &event)
{
	//const int modifiers = event.GetModifiers();
	const int key = event.GetKeyCode();
	//printf( "key down: %i %i", modifiers, key );
	switch( key) {
	case WXK_ESCAPE:
		{
			GetParent()->Hide();
		}
		break;
	case WXK_TAB:
		{
			if( 0 <= selected_item && selected_item < matching_items.size() ) {
				ChangeValue( matching_items[ selected_item ] );
				SetInsertionPointEnd();
				_popup->Hide();
			}
		}
		break;
	case WXK_UP:
		{
			if( selected_item > 0 ) {
				selected_item -= 1;
				if( selected_item < displayed_page_first_item ) { 
					displayed_page_first_item = selected_item;
				}
				_popup->Refresh();
			}
			if( 0 <= selected_item && selected_item < matching_items.size() ) {
				ChangeValue( matching_items[ selected_item ] );
				SetInsertionPointEnd();
			}
		}
		break;
	case WXK_DOWN:
		{
			if( selected_item < (matching_items.size() - 1) ) {
				selected_item += 1;

				const int show_at_least_this = selected_item - (displayed_page_num_items - 1);
				if( show_at_least_this > displayed_page_first_item ) {
					displayed_page_first_item = show_at_least_this;
				}
				_popup->Refresh();

			}
			if( 0 <= selected_item && selected_item < matching_items.size() ) {
				ChangeValue( matching_items[ selected_item ] );
				SetInsertionPointEnd();
			}
		}
		break;
	case WXK_RETURN:
		{
			_popup->Hide();
			event.Skip();
		}
	default:
		if( key != WXK_BACK && key != WXK_DELETE )
			_perform_autocomplete = true;
		event.Skip();
	}
}

void TaskDropDown::addItem( const char *item )
{
	if( items.Index(item) == wxNOT_FOUND ) {
		items.Add( item );
		items.Sort();
	}
}

void TaskDropDown::onFocus(wxFocusEvent &event)
{
	_popup->Hide();	
}
