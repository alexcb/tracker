#include "wx/app.h"
#include "wx/log.h"
#include "wx/frame.h"
#include "wx/panel.h"
#include "wx/stattext.h"
#include "wx/menu.h"
#include "wx/layout.h"
#include "wx/msgdlg.h"
#include "wx/icon.h"
#include "wx/button.h"
#include "wx/sizer.h"
#include "wx/textctrl.h"
#include "wx/settings.h"

#include "dropdown.h"

#ifdef WIN32
	#include "wx/window.h"
	
	//for parsing ini file
	#include <boost/property_tree/ptree.hpp>
	#include <boost/property_tree/ini_parser.hpp>

#else
	//headers for signal
	#include <signal.h>
#endif //WIN32


#include <sstream>
#include "tracker_ipc.h"
#include "inactivity.h"
#include "tracker_writer.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
	// override base class virtuals
	// ----------------------------

	// this one is called on application startup and is a good place for the app
	// initialization (doing it here and not in the ctor allows to have an error
	// return: if OnInit() returns false, the application terminates)
	virtual bool OnInit();
};

class MyPanel : public wxPanel
{
public:
	MyPanel(wxWindow *parent);

private:

	DECLARE_EVENT_TABLE()
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
	// ctor(s)
	MyFrame(const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	// event handlers (these functions should _not_ be virtual)
	//void OnAbout(wxCommandEvent& event);
	//void OnClearLog(wxCommandEvent& event);
	//void OnQuit(wxCommandEvent& event);

	void OnTextEnter(wxCommandEvent& event);
	void OnIdleStateChange( IdleDetectorEvent& event );
#ifdef WIN32
	void OnHotKeyPress( wxKeyEvent& event );
#endif //WIN32

	void SetLabelString(const char *str);
	void ShowTimeLogger();

private:
	//MyPanel *m_panel;
	//
	
	time_t _went_idle_at_time;
	std::string _current_task;
	time_t _current_task_start_time;

	TaskDropDown *_task_text_ctrl;
	wxStaticText *_task_static_text;

	// any class wishing to process wxWidgets events must use this macro
	DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_IDLE_STATE_CHANGE( MyFrame::OnIdleStateChange )
#ifdef WIN32
	EVT_HOTKEY( 1, MyFrame::OnHotKeyPress )
#endif //WIN32
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

//global for the single frame that our App runs
MyFrame *frame;

void MyFrame::ShowTimeLogger()
{
	std::ostringstream oss;
	if( _went_idle_at_time ) {
		time_t now = time(NULL);
		time_t seconds_idle = now - _went_idle_at_time;
		wxTimeSpan ts( 0, 0, seconds_idle, 0 );
		oss << "What were you doing while idle (" << ts.Format("%H hours %M Minutes %S seconds") << ")";
	} else {
		oss << "What are you doing?";
	}
	SetLabelString( oss.str().c_str() );

	_task_text_ctrl->ChangeValue( _current_task.c_str() );
	_task_text_ctrl->SetSelection( -1, -1 );

	Show(true);
	Raise();
}

static void signal_handler(int signo)
{
	frame->ShowTimeLogger();
}

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
	if ( !wxApp::OnInit() )
		return false;

//signalling only supported under linux (under windows, we register the global keycode so can always redisplay that way)
//under linux there's no way to catch global key controls, so instead a user must configure their window manager to run a new tracker instance
#ifdef WIN32
	//windows setups a hotkey instead (not supported under linux - linux must use an external program then signal the existing one)
	int hotkey_modifiers = 0;
	int hotkey_keycode = 0;
	std::string hotkey_string;
#else //Linux
	//signal other running instances
	if( signalOtherInstance() == true ) {
		//other process was signaled, so this instance should shutdown
		return false;
	}


	//otherwise, this is the single instance (first startup)
	if( signal( SIGUSR1, signal_handler) == SIG_ERR  )
	{
		printf("Parent: Unable to create handler for SIGUSR1\n");
	}

	writePID();
	//TODO need to delete this PID on shutdown
#endif


#ifdef WIN32
	//Read config values
	boost::property_tree::ptree pt;
	try {

		boost::property_tree::ini_parser::read_ini("tracker.ini", pt);
		std::string hotkey = pt.get<std::string>("hotkey.key", "enter");
		std::string mod_ctrl = pt.get<std::string>("hotkey.ctrl", "yes");
		std::string mod_shift = pt.get<std::string>("hotkey.shift", "yes");
		std::string mod_alt = pt.get<std::string>("hotkey.alt", "no");

		if( mod_ctrl == "yes" ) {
			hotkey_modifiers |= wxMOD_CONTROL;
			hotkey_string += "CTRL+";
		}
		if( mod_shift == "yes" ) {
			hotkey_modifiers |= wxMOD_SHIFT;
			hotkey_string += "SHIFT+";
		}
		if( mod_alt == "yes" ) {
			hotkey_modifiers |= wxMOD_ALT;
			hotkey_string += "ALT+";
		}

		if( hotkey.length() == 1 ) {
			hotkey_keycode = hotkey.c_str()[0];
			hotkey_string += hotkey.c_str()[0];
		} else if ( hotkey == "space" ) {
			hotkey_keycode = ' ';
			hotkey_string += "<space>";
		} else if ( hotkey == "enter" ) {
			hotkey_keycode = WXK_RETURN;
			hotkey_string += "<enter>";
		}

	} catch( const boost::property_tree::ini_parser::ini_parser_error& e ) {
		std::cout << "unable to open config: " << e.filename() << " " << e.message() << std::endl;
	} catch( const boost::property_tree::ptree_error& e ) {
		std::cout << "error fetching ini option: " << e.what() << std::endl;
	}
#endif //WIN32
	

	// Create the main application window
	frame = new MyFrame(wxT("Super Tracker")
								 ,wxPoint(50, 50), wxSize(450, 340)
								 );
#ifdef WIN32
	//bool ok = frame->RegisterHotKey( 1, wxMOD_CONTROL | wxMOD_SHIFT, ' ' );
	bool ok = frame->RegisterHotKey( 1, hotkey_modifiers, hotkey_keycode );
	if( !ok ) {
		std::ostringstream oss;
		oss << "unable to register hotkey: " << hotkey_string;
		wxMessageBox(oss.str().c_str());
	}
#endif //WIN32

	frame->ShowTimeLogger();
	//frame->Show(true);

	// success: wxApp::OnRun() will be called which will enter the main message
	// loop and the application will run. If we returned false here, the
	// application would exit immediately.
	return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame((wxFrame *)NULL, wxID_ANY, title, pos, size)
	  , _went_idle_at_time( 0 )
{
	const wxSizerFlags flags = wxSizerFlags().Centre().Border();

	IdleDetector* idleDetector = new IdleDetector();
	idleDetector->Start (this, wxTimeSpan (0, 5, 0, 0));

	wxSizer* sizer_top = new wxBoxSizer(wxVERTICAL);

	_task_text_ctrl = new TaskDropDown( this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize( 300, wxDefaultSize.GetY() ), wxTE_PROCESS_ENTER);
	_task_text_ctrl->Bind( wxEVT_COMMAND_TEXT_ENTER, &MyFrame::OnTextEnter, this );

	_task_static_text = new wxStaticText(this, wxID_ANY, wxT(""), wxDefaultPosition, wxSize( 300, wxDefaultSize.GetY() ));

	sizer_top->Add(_task_static_text, flags);
	sizer_top->Add(_task_text_ctrl, flags);

	SetSizerAndFit(sizer_top);

	//load items into drop down (this really shouldnt be here)
	std::set<std::string> tasks = getPreviousTasks();
	for( std::set<std::string>::const_iterator i = tasks.begin(); i != tasks.end(); i++ ) {
		_task_text_ctrl->addItem( i->c_str() );
	}

	//get last task and time
	_current_task = getLastTask( _current_task_start_time );
}

void MyFrame::SetLabelString(const char *str)
{
	time_t now = time(NULL);
	std::ostringstream oss;
	int minutes = (now - _current_task_start_time) / 60;
	int hours = minutes / 60;
	minutes = minutes % 60;

	oss << "Current task: " << _current_task << " (";

	//format time
	if( hours > 1 )
		oss << hours << " hours, ";
	else if( hours == 1 )
		oss << hours << " hour, ";
	if( minutes == 1 )
		oss << minutes << " minute";
	else
		oss << minutes << " minutes";

	oss << ")\n" << str;
	_task_static_text->SetLabel( oss.str().c_str() );
	GetSizer()->Fit(this);
}

void MyFrame::OnTextEnter(wxCommandEvent& event)
{
	wxString task = _task_text_ctrl->GetValue();
	_current_task_start_time = logTask( (const char*) task.c_str(), _went_idle_at_time );
	_task_text_ctrl->addItem( task );
	_went_idle_at_time = 0;
	_current_task = task.char_str();
	Hide();
}

void MyFrame::OnIdleStateChange( IdleDetectorEvent& event )
{
	if( event.IsIdle() ) {
		std::cout << "user is idle" << std::endl;
		if( _went_idle_at_time == 0 ) {
			//dont change unless the user has filled in what happened during the first idle time (to prevent mouse bumps)
			_went_idle_at_time = time(NULL) - event.GetIdleTime().GetSeconds().ToLong();
		}
	} else {
		std::cout << "user is back" << std::endl;
		ShowTimeLogger();
	}
}


#ifdef WIN32
void MyFrame::OnHotKeyPress( wxKeyEvent& event )
{
	ShowTimeLogger();
}
#endif //WIN32
