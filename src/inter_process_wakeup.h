#include <string>
#include <signal.h>

//Returns True when it works, False otherwise
bool signalOtherInstance(pid_t pid);

// Register a callback function (to be called when another process singals)
//Returns True when it works, False otherwise
bool setupWakeupHandler(void (*wakeup_handler)());

// Returns the full path to the PID file
std::string getPIDFilePath();

// Reads the PID stored in the file
pid_t readPID(const char *fname);

// Saves the current PID in the file
void writePID(const char *fname);

// Signal another instance if one is running; or register
// this instance as the single instance
// Returns True, if another instance was signaled (indicating this process should shutdown)
// or False, if no other process is running (indicating this process should continue)
bool signalOtherInstanceOrRegisterInstance(const char *fname);

