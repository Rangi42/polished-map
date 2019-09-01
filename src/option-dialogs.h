#ifndef OPTION_DIALOGS_H
#define OPTION_DIALOGS_H

#include <unordered_map>
#include <string>

#include "utils.h"
#include "config.h"
#include "widgets.h"
#include "tileset.h"
#include "map.h"

#pragma warning(push, 0)
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Widget.H>
#pragma warning(pop)

#define NEW_MAP_NAME "New Map"

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
	virtual void finish(void) {}
public:
	inline bool initialized(void) const { return !!_dialog; }
	void show(const Fl_Widget *p);
private:
	static void close_cb(Fl_Widget *, Option_Dialog *od);
	static void cancel_cb(Fl_Widget *, Option_Dialog *od);
};

typedef std::unordered_map<std::string, std::string> Dictionary;

class Map_Options_Dialog : public Option_Dialog {
private:
	int _max_tileset_name_length, _max_roof_name_length;
	Label *_map_header, *_map_size;
	OS_Spinner *_map_width, *_map_height;
	Dropdown *_map_sizes, *_tileset, *_roof;
	std::vector<std::pair<int, int>> _valid_sizes;
	Dictionary _original_names;
	friend class Tileset_Options_Dialog;
	friend class Roof_Options_Dialog;
public:
	Map_Options_Dialog(const char *t);
	~Map_Options_Dialog();
	bool limit_blk_options(const char *filename, const char *directory, Map_Attributes &attrs);
	inline uint8_t map_width(void) const { return (uint8_t)_map_width->value(); }
	inline uint8_t map_height(void) const { return (uint8_t)_map_height->value(); }
	const char *tileset(void) const;
	const char *roof(void) const;
	inline int num_roofs(void) const { return _roof->size() - 2; }
private:
	const char *original_name(const char *pretty_name) const;
	bool guess_map_size(const char *filename, const char *directory, Map_Attributes &attrs);
	std::string guess_map_tileset(const char *filename, const char *directory, Map_Attributes &attrs);
	void guess_tileset_names(const char *directory, Dictionary &pretty_names, Dictionary &guessable_names);
	std::string add_tileset(const char *t, int ext_len, const Dictionary &pretty_names);
	std::string add_roof(const char *r, int ext_len);
	int add_map_size(int w, int h);
	void add_valid_sizes(size_t n);
	void select_map_size(int i) { _map_sizes->value(i); _map_sizes->do_callback(); }
protected:
	void initialize_content(void);
	int refresh_content(int ww, int dy);
private:
	static inline bool valid_size(int w, int h) { return 1 <= w && w <= 255 && 1 <= h && h <= 255; }
	static void select_valid_size_cb(Dropdown *d, Map_Options_Dialog *md);
};

class Anchor_Button;

class Resize_Dialog : public Option_Dialog {
public:
	enum Hor_Align { LEFT, CENTER, RIGHT };
	enum Vert_Align { TOP, MIDDLE, BOTTOM };
private:
	Default_Spinner *_map_width, *_map_height;
	Anchor_Button *_anchor_buttons[9];
public:
	Resize_Dialog(const char *t);
	~Resize_Dialog();
	inline uint8_t map_width(void) const { return (uint8_t)_map_width->value(); }
	inline uint8_t map_height(void) const { return (uint8_t)_map_height->value(); }
	inline void map_size(uint8_t w, uint8_t h) { initialize(); _map_width->default_value(w); _map_height->default_value(h); }
	Hor_Align horizontal_anchor(void) const;
	Vert_Align vertical_anchor(void) const;
	int anchor(void) const;
	void anchor(int a);
	void anchor_label(int x, int y, const char *l);
protected:
	void initialize_content(void);
	int refresh_content(int ww, int dy);
private:
	static void anchor_button_cb(Anchor_Button *ab, Resize_Dialog *rd);
};

class Add_Sub_Dialog : public Option_Dialog {
private:
	Default_Spinner *_num_metatiles;
public:
	Add_Sub_Dialog(const char *t);
	~Add_Sub_Dialog();
	inline size_t num_metatiles(void) const { return (size_t)_num_metatiles->value(); }
	inline void num_metatiles(size_t n) { initialize(); _num_metatiles->default_value((double)n); }
protected:
	void initialize_content(void);
	int refresh_content(int ww, int dy);
};

class Tileset_Options_Dialog : public Option_Dialog {
private:
	Dropdown *_tileset;
	Map_Options_Dialog *_map_options_dialog;
public:
	Tileset_Options_Dialog(const char *t, Map_Options_Dialog *mod);
	~Tileset_Options_Dialog();
	bool limit_tileset_options(const char *old_tileset_name);
	const char *tileset(void) const;
protected:
	void initialize_content(void);
	int refresh_content(int ww, int dy);
};

class Roof_Options_Dialog : public Option_Dialog {
private:
	Dropdown *_roof;
	Map_Options_Dialog *_map_options_dialog;
public:
	Roof_Options_Dialog(const char *t, Map_Options_Dialog *mod);
	~Roof_Options_Dialog();
	bool limit_roof_options(const char *old_roof_name);
	const char *roof(void) const;
protected:
	void initialize_content(void);
	int refresh_content(int ww, int dy);
};

class Event_Options_Dialog : public Option_Dialog {
private:
	Label *_macro_heading, *_line_heading;
	OS_Input *_prefix, *_suffix;
	Default_Spinner *_event_x, *_event_y;
	Default_Hex_Spinner *_hex_event_x, *_hex_event_y;
public:
	Event_Options_Dialog(const char *t);
	~Event_Options_Dialog();
	void use_event(const Event *e);
	void update_event(Event *e);
protected:
	void initialize_content(void);
	int refresh_content(int ww, int dy);
};

class Print_Options_Dialog {
private:
	const char *_title;
	bool _copied, _canceled;
	Fl_Double_Window *_dialog;
	Label *_show_heading;
	OS_Check_Button *_grid, *_ids, *_priority, *_events;
	Default_Button *_export_button;
	OS_Button *_copy_button, *_cancel_button;
public:
	Print_Options_Dialog(const char *t = NULL);
	~Print_Options_Dialog();
	inline bool copied(void) const { return _copied; }
	inline void copied(bool c) { _copied = c; }
	inline bool canceled(void) const { return _canceled; }
	inline void canceled(bool c) { _canceled = c; }
	inline bool grid(void) { return !!_grid->value(); }
	inline void grid(bool g) { initialize(); _grid->value(g); }
	inline bool ids(void) { return !!_ids->value(); }
	inline void ids(bool d) { initialize(); _ids->value(d); }
	inline bool priority(void) { return !!_priority->value(); }
	inline void priority(bool p) { initialize(); _priority->value(p); }
	inline bool events(void) { return !!_events->value(); }
	inline void events(bool e) { initialize(); _events->value(e); }
private:
	void initialize(void);
	void refresh(void);
public:
	inline bool initialized(void) const { return !!_dialog; }
	void show(const Fl_Widget *p);
private:
	static void close_cb(Fl_Widget *, Print_Options_Dialog *pd);
	static void copy_cb(Fl_Widget *, Print_Options_Dialog *pd);
	static void cancel_cb(Fl_Widget *, Print_Options_Dialog *pd);
};
#endif
