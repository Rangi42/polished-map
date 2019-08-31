#ifndef BLOCK_WINDOW_H
#define BLOCK_WINDOW_H

#include <string>

#pragma warning(push, 0)
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#pragma warning(pop)

#include "tileset.h"
#include "map-buttons.h"
#include "metatile.h"
#include "widgets.h"

#define TILE_PX_SIZE (TILE_SIZE * ZOOM_FACTOR)

#define CHIP_ZOOM_FACTOR 4
#define CHIP_PX_SIZE (TILE_SIZE * CHIP_ZOOM_FACTOR)
#define CHIP_LINE_BYTES (CHIP_PX_SIZE * NUM_CHANNELS)

class Block_Window {
private:
	int _dx, _dy;
	const Tileset *_tileset;
	uint8_t _metatile_id;
	bool _canceled;
	Fl_Double_Window *_window;
	Label *_tileset_heading, *_tile_heading, *_metatile_heading, *_hover_tile_heading, *_collision_heading;
	Fl_Group *_tileset_group, *_current_group, *_metatile_group;
	Tile_Button *_tile_buttons[MAX_NUM_TILES], *_selected;
	Chip *_chips[METATILE_SIZE * METATILE_SIZE], *_current;
	Dropdown *_palette;
	OS_Check_Button *_x_flip, *_y_flip, *_extra, *_priority;
	OS_Input *_collision_inputs[NUM_QUADRANTS];
	Default_Hex_Spinner *_bin_collision_spinners[NUM_QUADRANTS];
	Default_Button *_ok_button;
	OS_Button *_cancel_button;
public:
	Block_Window(int x, int y);
	~Block_Window();
private:
	void initialize(void);
	void refresh(void);
public:
	inline const Tileset *tileset(void) const { return _tileset; }
	void tileset(const Tileset *t);
	void metatile(const Metatile *mt, bool has_collisions, bool bin_collisions);
	inline Chip *chip(int x, int y) { return _chips[y * METATILE_SIZE + x]; }
	inline const Chip *const_chip(int x, int y) const { return _chips[y * METATILE_SIZE + x]; }
	inline const char *collision(Quadrant q) { return _collision_inputs[q]->value(); }
	inline uint8_t bin_collision(Quadrant q) { return (uint8_t)_bin_collision_spinners[q]->value(); }
	inline bool canceled(void) const { return _canceled; }
	inline void canceled(bool c) { _canceled = c; }
	void show(const Fl_Widget *p);
	void select(const Attributable *a);
	void update_status(Chip *c);
private:
	static void close_cb(Fl_Widget *w, Block_Window *bw);
	static void cancel_cb(Fl_Widget *w, Block_Window *bw);
	static void select_tile_cb(Tile_Button *tb, Block_Window *bw);
	static void change_chip_cb(Chip *c, Block_Window *bw);
	static void change_attributes_cb(Fl_Widget *w, Block_Window *bw);
};

#endif
