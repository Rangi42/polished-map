#ifndef OPTION_DIALOGS_H
#define OPTION_DIALOGS_H

#include <unordered_map>
#include <string>

#include "utils.h"
#include "config.h"
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
	int _max_tileset_name_length;
	OS_Spinner *_map_width, *_map_height;
	Dropdown *_tileset, *_lighting;
	Dictionary _original_names;
	friend class Tileset_Options_Dialog;
	Config::Project _project_type;
public:
	Map_Options_Dialog(const char *t);
	~Map_Options_Dialog();
	bool limit_blk_options(const char *filename, const char *directory);
	inline uint8_t map_width(void) const { return (uint8_t)_map_width->value(); }
	inline uint8_t map_height(void) const { return (uint8_t)_map_height->value(); }
	const char *tileset(void) const;
	inline Lighting lighting(void) const { return (Lighting)_lighting->value(); }
private:
	const char *original_name(const char *pretty_name) const;
	bool guess_map_size(const char *filename, const char *directory, std::string &tileset_name);
	Dictionary guess_tileset_names(const char *directory);
	std::string add_tileset(const char *t, int ext_len, const Dictionary &pretty_names);
protected:
	void initialize_content(void);
	int refresh_content(int ww, int dy);
};

class Anchor_Button;

class Resize_Dialog : public Option_Dialog {
public:
	enum Hor_Align { LEFT, CENTER, RIGHT };
	enum Vert_Align { TOP, MIDDLE, BOTTOM };
private:
	OS_Spinner *_map_width, *_map_height;
	Anchor_Button *_anchor_buttons[9];
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
	void anchor_label(int x, int y, const char *l);
protected:
	void initialize_content(void);
	int refresh_content(int ww, int dy);
private:
	static void anchor_button_cb(Anchor_Button *ab, Resize_Dialog *rd);
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

class Tileset_Options_Dialog : public Option_Dialog {
private:
	Dropdown *_tileset, *_lighting;
	Map_Options_Dialog *_map_options_dialog;
public:
	Tileset_Options_Dialog(const char *t, Map_Options_Dialog *mod);
	~Tileset_Options_Dialog();
	bool limit_tileset_options(const char *old_tileset_name, Lighting old_lighting);
	const char *tileset(void) const;
	inline Lighting lighting(void) const { return (Lighting)_lighting->value(); }
protected:
	void initialize_content(void);
	int refresh_content(int ww, int dy);
	void finish(void);
};

#endif
