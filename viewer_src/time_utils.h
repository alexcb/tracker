#ifndef TIME_UTILS_H_SFNXLZ
#define TIME_UTILS_H_SFNXLZ

#include "wx/wx.h"

//t - epoch based UTC time
//returns seconds since midnight in local time
inline time_t getSecondsSinceLocalTimeMidnight(time_t t)
{
	wxDateTime date_time( t );
	time_t seconds_since_midnight = ( date_time.GetHour() * 60 + date_time.GetMinute() ) * 60 + date_time.GetSecond();
	return seconds_since_midnight;
}

//year = 2013
//month = 1..12
//day = 1..31
//TODO the is_dst flag doesnt work, UGH
inline wxDateTime dateTimeFromLocal(int year, int month, int day, int hour, int min, int sec, bool is_dst)
{
	month -= 1;
	wxDateTime date_time( (wxDateTime::wxDateTime_t)day, (wxDateTime::Month)month, year, hour, min, sec, 0);
	date_time.MakeTimezone(date_time.Local, is_dst);
	//TODO its possible to specify march 10 2013 2:30am which is not a valid time (as daylight savings kicks in and changes 2 -> 3am, therefore 2:xx doesnt exist)
	//TODO just like specifying dateTimeFromLocal(2013, 11, 3, hour, sec, 0, true) for 1:30am DST versus 1:30am non-DST isn't working 
	assert(date_time.IsValid());
	return date_time;
}

//inline time_t getEpochDayNumberLocalTime(time_t t)
//{
//	wxDateTime date_time( t );
//	tm = date_time.GetTm();
//	tm.tm_yday;
//	time_t seconds_since_midnight = ( date_time.GetHour() * 60 + date_time.GetMinute() ) * 60 + date_time.GetSecond();
//	return seconds_since_midnight;
//}

//round_up(4, 5) => 5
//round_up(17, 5) => 20
//round_up(25, 5) => 25
inline int round_up(int val, int interval)
{
	int remainder = val % interval;
	if( remainder )
		val += interval - remainder;
	return val;
}

#endif //TIME_UTILS_H_SFNXLZ