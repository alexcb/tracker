#include "string_util.h"
#include <string.h>

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