#ifdef WIN32
#include <Windows.h>
#else
#include <X11/extensions/scrnsaver.h> 

XScreenSaverInfo *screen_info = NULL;
Display *display = NULL;

#endif

const time_t getMilliSecondsSinceLastInput()
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

		return timeIdle;
	}
#else
	if( screen_info == NULL ) {
		screen_info = XScreenSaverAllocInfo();
		display = XOpenDisplay(0);
	}

	XScreenSaverQueryInfo(display, DefaultRootWindow(display), screen_info);
	return screen_info->idle;
#endif
	return 0;
}