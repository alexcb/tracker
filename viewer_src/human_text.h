#include <string>

//returns a human readable string from seconds to days, hours, minutes, and seconds
//if approx is true only return the highest unit
std::string human_time_elapsed(time_t seconds, bool approx=false);
