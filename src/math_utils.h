#ifndef MATH_UTILS_H_SFNZBQAL
#define MATH_UTILS_H_SFNZBQAL

inline int round_up(int val, int interval)
{
	int remainder = val % interval;
	if( remainder )
		val += interval - remainder;
	return val;
}

//returns true if lhs equals rhs +/- delta
inline bool approx_equal(int lhs, int rhs, int delta)
{
	return rhs - delta <= lhs && lhs <= rhs + delta;
}

#endif //MATH_UTILS_H_SFNZBQAL