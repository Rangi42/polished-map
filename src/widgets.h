#ifndef WIDGETS_H
#define WIDGETS_H

#include <string>

#pragma warning(push, 0)
#include <FL/Fl_Box.H>
#include <FL/Fl_Spinner.H>
#include <FL/Fl_Hor_Nice_Slider.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Radio_Round_Button.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Help_View.H>
#pragma warning(pop)

#define OS_SUBMENU(l) {l, 0, NULL, NULL, FL_SUBMENU, FL_NORMAL_LABEL, OS_FONT, OS_FONT_SIZE, FL_FOREGROUND_COLOR}
#define OS_MENU_ITEM(l, s, c, d, f) {l "    ", s, c, d, f, FL_NORMAL_LABEL, OS_FONT, OS_FONT_SIZE, FL_FOREGROUND_COLOR}

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

class OS_Repeat_Button : public Fl_Repeat_Button {
public:
	OS_Repeat_Button(int x, int y, int w, int h, const char *l = NULL);
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

class OS_Radio_Button : public Fl_Radio_Round_Button {
public:
	OS_Radio_Button(int x, int y, int w, int h, const char *l = NULL);
	void draw(void);
protected:
	int handle(int event);
};

class Link_Button : public OS_Button {
protected:
	Fl_Color _cached_color;
	Fl_Boxtype _cached_box;
public:
	Link_Button(int x, int y, int w, int h, const char *l = NULL);
public:
	void draw(void);
protected:
	int handle(int event);
};

class Expander_Collapser : public Fl_Check_Button {
public:
	Expander_Collapser(int x, int y, int w, int h, const char *l = NULL);
	void draw(void);
protected:
	int handle(int event);
};

class Toggle_Switch : public Fl_Check_Button {
private:
	const char *_off_text, *_on_text;
	Fl_Label _text_label;
public:
	Toggle_Switch(int x, int y, int w, int h, const char *l = NULL);
	inline const char *off_text(void) const { return _off_text; }
	inline void off_text(const char *t) { _off_text = t; }
	inline const char *on_text(void) const { return _on_text; }
	inline void on_text(const char *t) { _on_text = t; }
	inline Fl_Font textfont(void) const { return _text_label.font; }
	inline void textfont(Fl_Font f) { _text_label.font = f; }
	inline Fl_Fontsize textsize(void) const { return _text_label.size; }
	inline void textsize(Fl_Fontsize s) { _text_label.size = s; }
	inline Fl_Color textcolor(void) const { return _text_label.color; }
	inline void textcolor(Fl_Color c) { _text_label.color = c; }
	inline Fl_Align textalign(void) const { return _text_label.align_; }
	inline void textalign(Fl_Align a) { _text_label.align_ = a; }
	void draw(void);
protected:
	int handle(int event);
};

class OS_Spinner : public Fl_Spinner {
public:
	OS_Spinner(int x, int y, int w, int h, const char *l = NULL);
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

class Default_Value_Slider : public Default_Slider {
private:
	int _value_size;
	Fl_Font _textfont;
	Fl_Fontsize _textsize;
	Fl_Color _textcolor;
public:
	Default_Value_Slider(int x, int y, int w, int h, int vs, const char *l = NULL);
	inline Fl_Font textfont(void) const { return _textfont; }
	inline void textfont(Fl_Font f) { _textfont = f; }
	inline Fl_Fontsize textsize() const { return _textsize; }
	inline void textsize(Fl_Fontsize s) { _textsize = s; }
	inline Fl_Color textcolor() const { return _textcolor; }
	inline void textcolor(Fl_Color c) { _textcolor = c; }
protected:
	void draw(void);
	int handle(int event);
};

class OS_Progress : public Fl_Progress {
public:
	OS_Progress(int x, int y, int w, int h, const char *l = NULL);
};

class Indeterminate_Progress : public Fl_Widget {
public:
	static const int MAX_VALUE = 16;
	static const double BOUNCER_SIZE;
	static const double UPDATE_INTERVAL;
private:
	int _value;
public:
	Indeterminate_Progress(int x, int y, int w, int h, const char *l = NULL);
	~Indeterminate_Progress();
	inline int value(void) const { return _value; }
	inline void value(int v) { _value = v; damage(FL_DAMAGE_CHILD); }
protected:
	void draw(void);
	int handle(int event);
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

class Control_Group : public Fl_Group {
public:
	Control_Group(int x, int y, int w, int h, const char *l = NULL);
	void draw(void);
};

class Panel : public Fl_Group {
public:
	Panel(int x, int y, int w, int h, const char *l = NULL);
};

class OS_Tabs : public Fl_Tabs {
public:
	OS_Tabs(int x, int y, int w, int h, const char *l = NULL);
};

class OS_Tab : public Fl_Group {
public:
	OS_Tab(int x, int y, int w, int h, const char *l = NULL);
};

class OS_Scroll : public Fl_Scroll {
public:
	OS_Scroll(int x, int y, int w, int h, const char *l = NULL);
};

class Sidebar : public Fl_Group {
private:
	OS_Scroll _scroll;
public:
	Sidebar(int x, int y, int w, int h, const char *l = NULL);
	OS_Scroll &scroll(void) { return _scroll; }
};

class Toolbar : public Fl_Pack {
private:
	Fl_Group _spacer;
	int _alt_h;
public:
	Toolbar(int x, int y, int w, int h, const char *l = NULL);
	inline int alt_h(void) const { return _alt_h; }
	inline void alt_h(int h) { _alt_h = h; }
	void alternate(void);
	void resize(int x, int y, int w, int h);
	void end(void) { resize(x(), y(), w(), h()); }
};

class Alt_Widget {
protected:
	Fl_Image *_alt_image, *_alt_deimage, *_overlay_image, *_alt_overlay_image;
	int _alt_w, _alt_h;
	bool _alternated;
public:
	Alt_Widget(int w, int h);
	inline bool alternated(void) const { return _alternated; }
	inline int alt_w(void) const { return _alt_w; }
	inline int alt_h(void) const { return _alt_h; }
	inline void alt_size(int w, int h) { _alt_w = w; _alt_h = h; }
	inline Fl_Image *alt_image(void) { return _alt_image; }
	inline const Fl_Image *alt_image(void) const { return _alt_image; }
	inline void alt_image(Fl_Image *img) { _alt_image = img; }
	inline void alt_image(Fl_Image &img) { _alt_image = &img; }
	inline Fl_Image *alt_deimage(void) { return _alt_deimage; }
	inline const Fl_Image *alt_deimage(void) const { return _alt_deimage; }
	inline void alt_deimage(Fl_Image *img) { _alt_deimage = img; }
	inline void alt_deimage(Fl_Image &img) { _alt_deimage = &img; }
	inline Fl_Image *overlay_image(void) { return _overlay_image; }
	inline const Fl_Image *overlay_image(void) const { return _overlay_image; }
	inline void overlay_image(Fl_Image *img) { _overlay_image = img; }
	inline void overlay_image(Fl_Image &img) { _overlay_image = &img; }
	inline Fl_Image *alt_overlay_image(void) { return _alt_overlay_image; }
	inline const Fl_Image *alt_overlay_image(void) const { return _alt_overlay_image; }
	inline void alt_overlay_image(Fl_Image *img) { _alt_overlay_image = img; }
	inline void alt_overlay_image(Fl_Image &img) { _alt_overlay_image = &img; }
	virtual void alternate(void) = 0;
};

class Toolbar_Button : public Alt_Widget, public Fl_Button {
public:
	Toolbar_Button(int x, int y, int w, int h, const char *l = NULL);
protected:
	void alternate(void);
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

class Toolbar_Dropdown_Button : public Alt_Widget, public Fl_Menu_ {
protected:
	Fl_Boxtype _button_box;
public:
	Toolbar_Dropdown_Button(int x, int y, int w, int h, const char *l = NULL);
	const Fl_Menu_Item *dropdown(void);
	inline Fl_Boxtype button_box(void) const { return _button_box; }
	inline void button_box(Fl_Boxtype new_box) { _button_box = new_box; }
protected:
	void alternate(void);
	void draw(void);
	int handle(int event);
};

class Status_Bar_Field : public Fl_Box {
private:
	const char *_default_label;
public:
	Status_Bar_Field(int x, int y, int w, int h, const char *l = NULL);
	~Status_Bar_Field();
	inline const char *default_label(void) const { return _default_label; }
	inline void reset_label(void) { copy_label(_default_label); }
};

#endif
