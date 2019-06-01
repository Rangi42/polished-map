#ifndef GBSCREEN_H
#define GBSCREEN_H

#pragma warning(push, 0)
#include <FL/Fl_Box.H>
#pragma warning(pop)

#include "utils.h"
#include "widgets.h"

class GBScreen : public Fl_Box {
public:
	GBScreen();
	void draw(void);
	int handle(int event);
};

#endif
