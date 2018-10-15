#ifndef PREFERENCES_H
#define PREFERENCES_H

#pragma warning(push, 0)
#include <FL/Fl_Preferences.H>
#pragma warning(pop)

class Preferences {
public:
	static int get(const char *key, int default_ = 0);
	static void set(const char *key, int value);

};

#endif
