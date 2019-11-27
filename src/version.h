#ifndef VERSION_H
#define VERSION_H

#define PROGRAM_VERSION 4,4,3
#ifdef _DEBUG
#define PROGRAM_VERSION_STRING "4.4.3 [DEBUG]"
#else
#define PROGRAM_VERSION_STRING "4.4.3"
#endif

#define PROGRAM_EXE_NAME "polishedmap"

#ifdef _WIN32
#define PROGRAM_EXE PROGRAM_EXE_NAME ".exe"
#else
#define PROGRAM_EXE PROGRAM_EXE_NAME
#endif

#define PROGRAM_NAME "Polished Map"

#define PROGRAM_AUTHOR "Rangi"

#define CURRENT_YEAR "2019"

#endif
