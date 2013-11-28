#include "user_settings.h"

#include <sstream>
#include <fstream>
#include <stdexcept>

UserSettings::UserSettings() :
	num_minutes_before_idle( 5 ),
	auto_complete_show_tasks_newer_than_days( 60 )
{
#ifdef WIN32
	hot_key_code = "ctrl+alt+t";	
#endif //WIN32
}


void UserSettings::load( const char *file_path )
{
	_file_path = file_path;

	std::ifstream infile( file_path );
	if( infile.is_open() ) {
		infile >> num_minutes_before_idle;
		infile >> auto_complete_show_tasks_newer_than_days;
#ifdef WIN32
		infile >> hot_key_code;
#endif //WIN32

		std::string task;
		while( infile >> task )
			auto_complete_pre_populated_tasks.push_back( task );
	}
}


void UserSettings::save()
{
	FILE *fp = fopen( _file_path.c_str(), "w" );
	if( fp == NULL ) {
		throw std::runtime_error( "couldnt open file - (TODO insert the error here)" );
	}
	std::ostringstream oss;
	oss << num_minutes_before_idle << "\n";
	oss << auto_complete_show_tasks_newer_than_days << "\n";
#ifdef WIN32
	oss << hot_key_code << "\n";
#endif //WIN32
	
	for( int i = 0; i < auto_complete_pre_populated_tasks.size(); i++ )
		oss << auto_complete_pre_populated_tasks[ i ] << "\n";

	std::string to_write = oss.str();

	fwrite( to_write.c_str(), to_write.length(), 1, fp );
	fclose( fp );
	
	emit settingsChanged();
}
