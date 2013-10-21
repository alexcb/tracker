#include "human_text.h"

#include <sstream>

//////////////////////////////////////////////////////////
//For human_time_elapsed implementation

const int seconds_per_minute = 60;
const int seconds_per_hour = 60 * seconds_per_minute;
const int seconds_per_day = 24 * seconds_per_hour;
const int seconds_per_week = 7 * seconds_per_day;

const int seconds_per_unit[] = { 
	seconds_per_week, seconds_per_day, seconds_per_hour, seconds_per_minute, 1
};
const char* unit_names[] = { 
	"week", "day", "hour", "minute", "second"
};
const int num_units = 5;

void human_time_elapsed_helper( time_t &seconds, int seconds_per_unit, const char *unit_name, std::ostream &oss, bool &output_started )
{
	time_t num_units = seconds / seconds_per_unit;
	seconds = seconds % seconds_per_unit;
	if( num_units > 0 || output_started ) {
		if( output_started )
			oss << " ";
		output_started = true;
		oss << num_units << " " << unit_name;
		if( num_units != 1 )
			oss << "s";
	}
}

std::string human_time_elapsed(time_t seconds, bool approx)
{
	bool output_started = false;
	std::ostringstream oss;

	for( int i = 0; i < num_units && (approx == false || output_started == false); i++ )
		human_time_elapsed_helper( seconds, seconds_per_unit[i],   unit_names[i],   oss, output_started );

	return oss.str();
}
