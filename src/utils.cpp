#pragma warning(push, 0)
#include <FL/fl_draw.H>
#pragma warning(pop)

#include "utils.h"

int text_width(const char *l, int pad) {
	int lw = 0, lh = 0;
	fl_measure(l, lw, lh, 0);
	return lw + 2 * pad;
}
