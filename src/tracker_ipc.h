#ifndef TRACKER_IPC_H_FSVXNLZ
#define TRACKER_IPC_H_FSVXNLZ

#ifndef WIN32

//returns true if the wakeup signal is correctly sent, false otherwise
bool signalOtherInstance();

void writePID();

#endif //WIN32

#endif //TRACKER_IPC_H_FSVXNLZ
