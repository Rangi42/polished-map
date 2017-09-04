#ifndef OPTION_DIALOGS_H
#define OPTION_DIALOGS_H

#include "utils.h"
#include "widgets.h"
#include "tileset.h"

#pragma warning(push, 0)
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Widget.H>
#pragma warning(pop)

class Option_Dialog {
protected:
	const char *_title;
	bool _canceled;
	Fl_Double_Window *_dialog;
	Fl_Group *_content;
	Default_Button *_ok_button;
	OS_Button *_cancel_button;
public:
	Option_Dialog(const char *t = NULL);
	virtual ~Option_Dialog();
	inline bool canceled(void) const { return _canceled; }
	inline void canceled(bool c) { _canceled = c; }
protected:
	void initialize(void);
	void refresh(void);
	virtual void initialize_content(void) = 0;
	virtual int refresh_content(int ww, int dy) = 0;
public:
	void show(const Fl_Widget *p);
private:
	static void close_cb(Fl_Widget *, Option_Dialog *od);
	static void cancel_cb(Fl_Widget *, Option_Dialog *od);
};

class Map_Options_Dialog : public Option_Dialog {
private:
	OS_Spinner *_map_width, *_map_height;
	Dropdown *_tileset, *_lighting;
public:
	Map_Options_Dialog(const char *t);
	~Map_Options_Dialog();
	bool limit_blk_options(const char *f);
	inline uint8_t map_width(void) const { return (uint8_t)_map_width->value(); }
	inline uint8_t map_height(void) const { return (uint8_t)_map_height->value(); }
	inline const char *tileset(void) const { return _tileset->mvalue() ? _tileset->mvalue()->label() : NULL; }
	inline Tileset::Lighting lighting(void) const { return (Tileset::Lighting)_lighting->value(); }
protected:
	void initialize_content(void);
	int refresh_content(int ww, int dy);
};

/*class Resize_Dialog : public Option_Dialog {
private:
	OS_Spinner *_map_width, *_map_height;
	Label *_anchor_heading;
	Fl_Group *_anchor_vertical, *_anchor_horizontal;
	OS_Radio_Button *_anchor_top, *_anchor_bottom, *_anchor_left, *_anchor_right;
public:
	Resize_Dialog(const char *t);
	~Resize_Dialog();
	inline uint8_t map_width(void) const { return (uint8_t)_map_width->value(); }
	inline uint8_t map_height(void) const { return (uint8_t)_map_height->value(); }
	inline bool anchor_vertical_top(void) const { return !!_anchor_top->value(); }
	inline bool anchor_horizontal_left(void) const { return !!_anchor_left->value(); }
protected:
	void initialize_content(void);
	int refresh_content(int ww, int dy);
};*/

#endif
