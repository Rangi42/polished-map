#ifndef GAME_BOY_SCREEN_H
#define GAME_BOY_SCREEN_H

#pragma warning(push, 0)
#include <FL/Fl_Box.H>
#pragma warning(pop)

#include "utils.h"
#include "widgets.h"

class Game_Boy_Screen : public Fl_Box {
public:
	Game_Boy_Screen();
	void draw(void);
	inline int handle(int) { return 0; }
};

#endif
