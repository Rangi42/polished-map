#ifndef SCREEN_H
#define SCREEN_H

#pragma warning(push, 0)
#include <FL/Fl_Box.H>
#pragma warning(pop)

#include "utils.h"
#include "widgets.h"

class Screen : public Fl_Box {
public:
	Screen();
	void draw(void);
	int handle(int event);
};

#endif
