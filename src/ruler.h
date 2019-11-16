#ifndef RULER_H
#define RULER_H

#include <cstdlib>

#pragma warning(push, 0)
#include <FL/Fl_Box.H>
#pragma warning(pop)

class Ruler : public Fl_Box {
public:
	enum class Direction { CORNER, HORIZONTAL, VERTICAL };
private:
	Direction _direction;
public:
	Ruler(int x, int y, int w, int h, const char *l = NULL);
	inline Direction direction(void) const { return _direction; }
	inline void direction(Direction d) { _direction = d; }
	void draw(void);
};

#endif
