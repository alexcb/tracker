//Nearly all of this is from http://utmostlogic.wordpress.com/tag/wxwidgets/

#include "inactivity.h"

#ifdef WIN32
#include <Windows.h>
#include "wx/msw/winundef.h"
#else
#include <X11/extensions/scrnsaver.h> 

XScreenSaverInfo *screen_info = NULL;
Display *display = NULL;

#endif

DEFINE_EVENT_TYPE(EVT_IDLE_STATE_CHANGE)
IMPLEMENT_DYNAMIC_CLASS(IdleDetectorEvent, wxEvent)
#define IDLE_TO_ACTIVE_TIMEOUT  1000
const wxTimeSpan IdleDetector::GetIdleTime()
{
#ifdef WIN32
	LASTINPUTINFO   info;
	info.cbSize = sizeof(info);
	if (::GetLastInputInfo (&info))
	{
		long timeIdle = ::GetTickCount() - info.dwTime;
		// Handle overflow
		if (timeIdle < 0)
			timeIdle = timeIdle + UINT_MAX + 1;

		return wxTimeSpan(0, 0, 0, timeIdle);
	}
#else
	if( screen_info == NULL ) {
		screen_info = XScreenSaverAllocInfo();
		display = XOpenDisplay(0);
	}

	XScreenSaverQueryInfo(display, DefaultRootWindow(display), screen_info);
	return wxTimeSpan( 0, 0, 0, screen_info->idle );
#endif
	return wxTimeSpan ();
}
void IdleDetector::Start (wxEvtHandler* owner, const wxTimeSpan& timeout)
{
	SetOwner (owner);
	m_timeout = timeout;
	m_isIdle = false;
	SetState (false, wxTimeSpan());
}
void IdleDetector::Pause ()
{
	wxTimer::Stop ();
}
void IdleDetector::Resume ()
{
	SetState (m_isIdle, GetIdleTime());
}
bool IdleDetector::IsIdle () const
{
	return (IsRunning () ? m_isIdle : false);
}
void IdleDetector::Notify ()
{
	wxTimeSpan idleTime = GetIdleTime ();
	//
	// If not currently idle, then the state changes if the amount of time that
	// the session has been idle is greater than or equal to the idle timeout.
	//
	// If currently idle, then the state changes if the amount of time that
	// the session has been idle is less than the idle timeout. This assumes
	// that the idle timeout value is greater than the IDLE_TO_ACTIVE_TIMEOUT
	// timeout value, which it really should be.
	//
	if (!m_isIdle)
		SetState (idleTime >= m_timeout, idleTime);
	else if (idleTime < m_timeout)
		SetState (false, idleTime);
}
void IdleDetector::SetState (bool isIdle, const wxTimeSpan& idleTime)
{
	if (isIdle != m_isIdle)
	{
		m_isIdle = isIdle;
		IdleDetectorEvent   event(EVT_IDLE_STATE_CHANGE, isIdle, idleTime);
		event.SetEventObject (this);
		wxPostEvent( GetOwner(), event );
	}
	if (m_isIdle)
		wxTimer::Start (IDLE_TO_ACTIVE_TIMEOUT, wxTIMER_CONTINUOUS);
	else
	{
		wxTimeSpan  timeRemaining = m_timeout - idleTime;
		wxTimer::Start (timeRemaining.IsPositive() ? timeRemaining.GetMilliseconds().ToLong() : 10, wxTIMER_ONE_SHOT);
	}
}
