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
	int _width;
	const char *_title;
	bool _canceled;
	Fl_Double_Window *_dialog;
	Fl_Group *_content;
	Default_Button *_ok_button;
	OS_Button *_cancel_button;
public:
	Option_Dialog(int w, const char *t = NULL);
	virtual ~Option_Dialog();
	inline bool canceled(void) const { return _canceled; }
	inline void canceled(bool c) { _canceled = c; }
protected:
	void initialize(void);
	void refresh(void);
	virtual void initialize_content(void) = 0;
	virtual int refresh_content(int ww, int dy) = 0;
public:
	inline bool initialized(void) const { return !!_dialog; }
	void show(const Fl_Widget *p);
private:
	static void close_cb(Fl_Widget *, Option_Dialog *od);
	static void cancel_cb(Fl_Widget *, Option_Dialog *od);
};

class Map_Options_Dialog : public Option_Dialog {
private:
	int _max_tileset_name_length;
	OS_Spinner *_map_width, *_map_height;
	Dropdown *_tileset, *_lighting;
	OS_Check_Button *_skip_60_7f;
public:
	Map_Options_Dialog(const char *t);
	~Map_Options_Dialog();
	bool limit_blk_options(const char *filename, const char *directory);
	inline uint8_t map_width(void) const { return (uint8_t)_map_width->value(); }
	inline uint8_t map_height(void) const { return (uint8_t)_map_height->value(); }
	inline const char *tileset(void) const { return _tileset->mvalue() ? _tileset->mvalue()->label() : NULL; }
	inline Tileset::Lighting lighting(void) const { return (Tileset::Lighting)_lighting->value(); }
	inline bool skip_60_7f(void) const { return !!_skip_60_7f->value(); }
private:
	bool guess_map_size(const char *filename, const char *directory);
protected:
	void initialize_content(void);
	int refresh_content(int ww, int dy);
};

class Resize_Dialog : public Option_Dialog {
public:
	enum Hor_Align { LEFT, CENTER, RIGHT };
	enum Vert_Align { TOP, MIDDLE, BOTTOM };
private:
	OS_Spinner *_map_width, *_map_height;
	OS_Button *_anchor_top_left, *_anchor_top_center, *_anchor_top_right,
		*_anchor_middle_left, *_anchor_middle_center, *_anchor_middle_right,
		*_anchor_bottom_left, *_anchor_bottom_center, *_anchor_bottom_right;
public:
	Resize_Dialog(const char *t);
	~Resize_Dialog();
	inline uint8_t map_width(void) const { return (uint8_t)_map_width->value(); }
	inline uint8_t map_height(void) const { return (uint8_t)_map_height->value(); }
	inline void map_size(uint8_t w, uint8_t h) { initialize(); _map_width->value(w); _map_height->value(h); }
	Hor_Align horizontal_anchor(void) const;
	Vert_Align vertical_anchor(void) const;
	int anchor(void) const;
	void anchor(int a);
protected:
	void initialize_content(void);
	int refresh_content(int ww, int dy);
};

class Add_Sub_Dialog : public Option_Dialog {
private:
	OS_Spinner *_num_metatiles;
public:
	Add_Sub_Dialog(const char *t);
	~Add_Sub_Dialog();
	inline size_t num_metatiles(void) const { return (size_t)_num_metatiles->value(); }
	inline void num_metatiles(size_t n) { initialize(); _num_metatiles->value((double)n); }
protected:
	void initialize_content(void);
	int refresh_content(int ww, int dy);
};

#endif
