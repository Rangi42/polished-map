#ifndef WAITING_DIALOG_H
#define WAITING_DIALOG_H

#include <ShObjIdl.h>

#pragma warning(push, 0)
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#pragma warning(pop)

#include "widgets.h"

class Waiting_Dialog {
private:
	const char *_title;
	bool _canceled;
	Fl_Window *_top_window;
	Fl_Double_Window *_dialog;
	Label *_body;
	Indeterminate_Progress *_progress;
	Default_Button *_cancel_button;
public:
	Waiting_Dialog(Fl_Window *top, const char *t = NULL);
	~Waiting_Dialog();
private:
	void initialize(void);
	void refresh(void);
	ITaskbarList3 *_taskbar_list3;
	bool _taskbar_failed;
	bool initialize_taskbar(void);
public:
	inline void title(const char *t) { _title = t; if (_dialog) { _dialog->copy_label(_title); } }
	inline void message(const char *m) { _body->copy_label(m); }
	inline bool canceled(void) const { return _canceled; }
	inline void canceled(bool c) { _canceled = c; }
	void show(const Fl_Widget *p);
	void hide(void);
private:
	static void close_cb(Fl_Widget *w, Waiting_Dialog *wd);
	static void cancel_cb(Fl_Widget *w, Waiting_Dialog *wd);
};

#endif
