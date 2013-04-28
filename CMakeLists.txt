# define minimum cmake version
cmake_minimum_required(VERSION 2.6.2)
 
# Our project is called 'minimal' this is how it will be called in
# visual studio, and in our makefiles. 
project(tracker)
 
# Location where cmake first looks for cmake modules.
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")
 
##---------------------------------------------------
## Please set your wxWidgets configuration here
##--------------------------------------------------- 
 
# Here you can define what libraries of wxWidgets you need for your
# application. You can figure out what libraries you need here;
# http://www.wxwidgets.org/manuals/2.8/wx_librarieslist.html
# We need the Find package for wxWidgets to work
# NOTE: if you're using aui, include aui in this required components list.
 
SET(LIBRARIES "")
 
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	#need to manually setup up paths here

	set(WXWIDGETS_INCLUDES "C:\\wx\\wxWidgets-2.9.4\\include" "C:\\wx\\wxWidgets-2.9.4\\lib\\vc_lib\\mswud")
	set(WXWIDGETS_LIBS "C:\\wx\\wxWidgets-2.9.4\\lib\\vc_lib")
	
	set(BOOST_INCLUDES "C:\\boost\\boost_1_53_0")
	
	#add_definition(-DCOMPILE_UNIT_TESTS)
	
	include_directories(${WXWIDGETS_INCLUDES} ${BOOST_INCLUDES})
	link_directories(${WXWIDGETS_LIBS})
	
	SET(WIN32 "WIN32") #tell visual studios to build in windows subsystem rather than console
	
	SET(LIBRARIES
		wxbase29ud
		wxmsw29ud_gl
		wxmsw29ud_webview
		wxbase29ud_net
		wxmsw29ud_html
		wxmsw29ud_xrc
		wxbase29ud_xml
		wxmsw29ud_media
		wxpngd
		wxexpatd
		wxmsw29ud_propgrid
		wxregexud
		wxjpegd
		wxmsw29ud_qa
		wxscintillad
		wxmsw29ud_adv
		wxmsw29ud_ribbon
		wxtiffd
		wxmsw29ud_aui
		wxmsw29ud_richtext
		wxzlibd
		wxmsw29ud_core
		wxmsw29ud_stc
		comctl32
		rpcrt4
		winmm
		advapi32
		wsock32
	)
	
else(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	#On linux, the find package wxwidgets just works
	
	# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.
	find_package(wxWidgets COMPONENTS core base REQUIRED)
 
	# wxWidgets include (this will do all the magic to configure everything)
	include( "${wxWidgets_USE_FILE}" )

	
	#TODO set library Xss (for getting idle time)
	SET(LIBRARIES
		Xss
		${wxWidgets_LIBRARIES}
	)

endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
 
# For convenience we define the sources as a variable. You can add 
# header files and cpp/c files and CMake will sort them out
file(GLOB SRCS 
	"src/*.cpp"
)
 
# Here we define the executable minimal.exe or minimal on other systems
# the above paths and defines will be used in this build
add_executable(tracker ${WIN32} ${SRCS})
#include_directories(${CMAKE_SOURCE_DIR}/lib1/include)
#target_link_libraries(mylib ${lib1_path})
 
# We add to our target 'minimal' the wxWidgets libraries. These are
# set for us by the find script. If you need other libraries, you
# can add them here as well. 
#target_link_libraries(tracker ${wxWidgets_LIBRARIES} Xss)

target_link_libraries(tracker ${LIBRARIES})

#######################################################################
### Viewer program:

file(GLOB VIEWER_SRCS 
	"viewer_src/*.cpp"
)
add_executable(viewer ${WIN32} ${VIEWER_SRCS})
target_link_libraries(viewer ${LIBRARIES})