#ifndef GAME_BOY_SCREEN_H
#define GAME_BOY_SCREEN_H

class Game_Boy_Screen {
private:
	static int _x, _y, _w, _h;
public:
	inline static void resize(int x, int y, int w, int h) { _x = x; _y = y; _w = w; _h = h; }
	static void draw(void);
};

#endif
