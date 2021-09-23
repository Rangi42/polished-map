#ifndef WIDGETS_H
#define WIDGETS_H

#include <string>
#include <vector>

#pragma warning(push, 0)
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Spinner.H>
#include <FL/Fl_Hor_Nice_Slider.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Radio_Round_Button.H>
#include <FL/Fl_Help_View.H>
#pragma warning(pop)

#include "hex-spinner.h"

#define OS_MENU_ITEM_PREFIX " "
#define OS_MENU_ITEM_SUFFIX "         "

#define OS_SUBMENU(l) {l, 0, NULL, NULL, FL_SUBMENU, FL_NORMAL_LABEL, OS_FONT, OS_FONT_SIZE, FL_FOREGROUND_COLOR}
#define OS_NULL_MENU_ITEM(s, c, d, f) {"", s, c, d, f, FL_NORMAL_LABEL, OS_FONT, OS_FONT_SIZE, FL_FOREGROUND_COLOR}
#define OS_MENU_ITEM(l, s, c, d, f) {OS_MENU_ITEM_PREFIX l OS_MENU_ITEM_SUFFIX, s, c, d, f, FL_NORMAL_LABEL, OS_FONT, OS_FONT_SIZE, FL_FOREGROUND_COLOR}

class DnD_Receiver : public Fl_Box {
public:
	static void deferred_callback(DnD_Receiver *dndr);
private:
	std::string _text;
public:
	DnD_Receiver(int x, int y, int w, int h, const char *l = NULL);
	inline const std::string &text(void) const { return _text; }
	int handle(int event);
};

class Label : public Fl_Box {
public:
	Label(int x, int y, int w, int h, const char *l = NULL);
};

class Spacer : public Fl_Box {
public:
	Spacer(int x, int y, int w, int h, const char *l = NULL);
};

class OS_Input : public Fl_Input {
public:
	OS_Input(int x, int y, int w, int h, const char *l = NULL);
};

class OS_Hex_Input : public Hex_Input {
public:
	OS_Hex_Input(int x, int y, int w, int h, const char *l = NULL);
};

class OS_Button : public Fl_Button {
public:
	OS_Button(int x, int y, int w, int h, const char *l = NULL);
protected:
	int handle(int event);
};

class Default_Button : public Fl_Button {
public:
	Default_Button(int x, int y, int w, int h, const char *l = NULL);
protected:
	int handle(int event);
};

class OS_Check_Button : public Fl_Check_Button {
public:
	OS_Check_Button(int x, int y, int w, int h, const char *l = NULL);
	void draw(void);
protected:
	int handle(int event);
};

class OS_Spinner : public Fl_Spinner {
public:
	OS_Spinner(int x, int y, int w, int h, const char *l = NULL);
};

class OS_Hex_Spinner : public Hex_Spinner {
public:
	OS_Hex_Spinner(int x, int y, int w, int h, const char *l = NULL);
};

class Default_Spinner : public OS_Spinner {
private:
	double _default_value;
public:
	Default_Spinner(int x, int y, int w, int h, const char *l = NULL);
	inline double default_value(void) const { return _default_value; }
	inline void default_value(double v) { _default_value = v; value(_default_value); }
protected:
	int handle(int event);
};

class Default_Hex_Spinner : public OS_Hex_Spinner {
private:
	int _default_value;
public:
	Default_Hex_Spinner(int x, int y, int w, int h, const char *l = NULL);
	inline int default_value(void) const { return _default_value; }
	inline void default_value(int v) { _default_value = v; value(_default_value); }
protected:
	int handle(int event);
};

class OS_Slider : public Fl_Hor_Nice_Slider {
public:
	OS_Slider(int x, int y, int w, int h, const char *l = NULL);
	void draw(void);
	void draw(int x, int y, int w, int h);
protected:
	int handle(int event);
};

class Default_Slider : public OS_Slider {
private:
	double _default_value;
public:
	Default_Slider(int x, int y, int w, int h, const char *l = NULL);
	inline double default_value(void) const { return _default_value; }
	inline void default_value(double v) { _default_value = v; value(_default_value); }
protected:
	int handle(int event);
	int handle(int event, int x, int y, int w, int h);
};

class HTML_View : public Fl_Help_View {
public:
	HTML_View(int x, int y, int w, int h, const char *l = NULL);
};

class Dropdown : public Fl_Choice {
public:
	Dropdown(int x, int y, int w, int h, const char *l = NULL);
	void draw(void);
};

class OS_Scroll : public Fl_Scroll {
public:
	OS_Scroll(int x, int y, int w, int h, const char *l = NULL);
};

class Workspace : public OS_Scroll {
private:
	int _content_w, _content_h;
	int _ox, _oy, _cx, _cy;
	DnD_Receiver *_dnd_receiver;
	std::vector<Fl_Widget *> _correlates;
public:
	Workspace(int x, int y, int w, int h, const char *l = NULL);
	inline void contents(int w, int h) { _content_w = w; _content_h = h; }
	inline bool has_x_scroll(void) const { return !!hscrollbar.visible(); }
	inline bool has_y_scroll(void) const { return !!scrollbar.visible(); }
	inline void dnd_receiver(DnD_Receiver *dndr) { _dnd_receiver = dndr; }
	inline void add_correlate(Fl_Widget *wgt) { _correlates.push_back(wgt); }
	inline void clear_correlates(void) { _correlates.clear(); }
	int handle(int event);
	void scroll_to(int x, int y);
private:
	static void hscrollbar_cb(Fl_Scrollbar *sb, void *);
	static void scrollbar_cb(Fl_Scrollbar *sb, void *);
};

class Toolbar : public Fl_Group {
private:
	Fl_Group _spacer;
public:
	Toolbar(int x, int y, int w, int h, const char *l = NULL);
	void resize(int x, int y, int w, int h);
	void draw(void);
	void end(void) { resize(x(), y(), w(), h()); }
};

class Toolbar_Button : public Fl_Button {
public:
	Toolbar_Button(int x, int y, int w, int h, const char *l = NULL);
protected:
	void draw(void);
	int handle(int event);
};

class Toolbar_Toggle_Button : public Toolbar_Button {
public:
	Toolbar_Toggle_Button(int x, int y, int w, int h, const char *l = NULL);
};

class Toolbar_Radio_Button : public Toolbar_Button {
public:
	Toolbar_Radio_Button(int x, int y, int w, int h, const char *l = NULL);
};

#endif
