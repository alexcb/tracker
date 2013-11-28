cmake_minimum_required(VERSION 2.8.9)

PROJECT(tracker)

#TODO set this to build (figure out whats needed on linux, if anything)
#SET(CMAKE_PREFIX_PATH C:/qt/Qt5.0.2/5.0.2/msvc2010)

#build a win32 app (rather than console)
SET(WIN32 "WIN32")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Widgets)


file(GLOB tracker_SOURCES
	"src/*.cpp"
)
file(GLOB tracker_HEADERS
	"src/*.h"
)

ADD_EXECUTABLE(tracker ${WIN32} ${tracker_HEADERS} ${tracker_SOURCES})
qt5_use_modules(tracker LINK_PRIVATE Widgets)

TARGET_LINK_LIBRARIES(tracker Xss X11)


MESSAGE("====================================================")
MESSAGE(" IMPORTANT USER ACTION REQUIRED                     ")
MESSAGE("====================================================")
MESSAGE("To run tracker under visual studio debugger, set    ")
MESSAGE("tracker->properties->Debugging->Environment         ")
MESSAGE("to                                                  ")
MESSAGE("PATH=${CMAKE_PREFIX_PATH}/bin;%PATH%                ")
MESSAGE("====================================================")
