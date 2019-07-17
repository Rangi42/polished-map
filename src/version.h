#ifndef VERSION_H
#define VERSION_H

#define PROGRAM_VERSION 2,3,0
#ifdef _DEBUG
#define PROGRAM_VERSION_STRING "2.3.0 [DEBUG]"
#else
#define PROGRAM_VERSION_STRING "2.3.0"
#endif

#ifdef _WIN32
#define PROGRAM_EXE "polishedmap-plusplus.exe"
#else
#define PROGRAM_EXE "polishedmap-plusplus"
#endif

#define PROGRAM_NAME "Polished Map++"

#define PROGRAM_AUTHOR "Rangi"

#define CURRENT_YEAR "2019"

#endif
