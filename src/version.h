#ifndef VERSION_H
#define VERSION_H

#define PROGRAM_VERSION 4,1,0
#ifdef _DEBUG
#define PROGRAM_VERSION_STRING "4.1.0 [DEBUG]"
#else
#define PROGRAM_VERSION_STRING "4.1.0"
#endif

#ifdef _WIN32
#define PROGRAM_EXE "polishedmap.exe"
#else
#define PROGRAM_EXE "polishedmap"
#endif

#define PROGRAM_NAME "Polished Map"

#define PROGRAM_AUTHOR "Rangi"

#define CURRENT_YEAR "2019"

#endif
