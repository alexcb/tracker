#include "string_util.h"
#include <string.h>
#include <algorithm>
#include <string> 

int numberOfStringMatches( const char *string, const char *term )
{
	int num_found = 0;
	while( string ) {
		string = strstr(string, term);
		if( string ) {
			string++;
			num_found++;
		}
	}
	return num_found;
}

const char* findNthString( const char *s, const char *term, int n )
{
	while( n > 0 && s ) {
		s = strstr(s, term);
		n--;
		if( n > 0 && s )
			s++;
	}
	return s;
}

const char* strstri( const char* str1, const char* str2 )
{
	std::string cpp_str_1( str1 );
	std::transform( cpp_str_1.begin(), cpp_str_1.end(), cpp_str_1.begin(), ::tolower);

	std::string cpp_str_2( str2 );
	std::transform( cpp_str_2.begin(), cpp_str_2.end(), cpp_str_2.begin(), ::tolower);

	const char *p = strstr( cpp_str_1.c_str(), cpp_str_2.c_str() );
	if( p ) {
		unsigned int offset = p - cpp_str_1.c_str();
		p = str1 + offset;
	}
	return p;
}
