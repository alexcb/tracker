unsigned long getIdleTime();

//Nearly all of this is from http://utmostlogic.wordpress.com/tag/wxwidgets/


#ifdef WIN32
#include "wx/window.h"
#endif //WIN32

//#include "wx/datetime.h"
#include "wx/timer.h"

class IdleDetectorEvent : public wxEvent
{
public:
    IdleDetectorEvent(wxEventType type = wxEVT_NULL, bool isIdle = true, const wxTimeSpan& idleTime=wxTimeSpan())
        : wxEvent(0, type), m_isIdle(isIdle), m_idleTime(idleTime)
    {}
    IdleDetectorEvent(const IdleDetectorEvent& event)
        : wxEvent(event), m_isIdle(event.m_isIdle), m_idleTime(event.m_idleTime)
    {}
    bool IsIdle() const { return m_isIdle; }
    const wxTimeSpan& GetIdleTime() const { return m_idleTime; }
    virtual wxEvent *Clone() const { return new IdleDetectorEvent(*this); }
private:
    bool        m_isIdle;
    wxTimeSpan  m_idleTime;
private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(IdleDetectorEvent)
};
BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(EVT_IDLE_STATE_CHANGE, -1)
END_DECLARE_EVENT_TYPES()
typedef void (wxEvtHandler::*IdleDetectorEventFunction) (IdleDetectorEvent&);
#define EVT_IDLE_STATE_CHANGE(func)  DECLARE_EVENT_TABLE_ENTRY( EVT_IDLE_STATE_CHANGE, wxID_ANY, wxID_ANY, IdleDetectorEventHandler(func), NULL ),
#define IdleDetectorEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(IdleDetectorEventFunction, &func)

class IdleDetector : public wxTimer
{
public:
    IdleDetector () : m_isIdle(false) {}
    const wxTimeSpan& GetTimeout () const { return m_timeout; }
    void Start (wxEvtHandler* owner, const wxTimeSpan& timeout);
    void Pause ();
    void Resume ();
    bool    IsIdle () const;
    static const wxTimeSpan GetIdleTime ();
private:
    void    Notify ();
    void    SetState (bool isIdle, const wxTimeSpan& idleTime);
private:
    wxTimeSpan      m_timeout;
    bool            m_isIdle;
};
