#include <string>

#pragma warning(push, 0)
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/fl_draw.H>
#pragma warning(pop)

#include "themes.h"
#include "widgets.h"
#include "progress-dialog.h"

Progress_Dialog::Progress_Dialog(Fl_Window *top, const char *t) : _title(t), _canceled(false), _top_window(top),
	_dialog(NULL), _body(NULL), _progress(NULL), _cancel_button(NULL), _taskbar_list3(NULL), _taskbar_failed(false)
	{}

Progress_Dialog::~Progress_Dialog() {
	_top_window = NULL;
	delete _dialog;
	delete _body;
	delete _progress;
	delete _cancel_button;
	if (_taskbar_list3) {
		_taskbar_list3->Release();
		CoUninitialize();
	}
}

void Progress_Dialog::initialize() {
	if (_dialog) { return; }
	Fl_Group *prev_current = Fl_Group::current();
	Fl_Group::current(NULL);
	// Populate dialog
	_dialog = new Fl_Double_Window(0, 0, 0, 0, _title);
	_body = new Label(0, 0, 0, 0);
	_progress = new OS_Progress(0, 0, 0, 0);
	_cancel_button = new Default_Button(0, 0, 0, 0, "Cancel");
	_dialog->end();
	// Initialize dialog
	_dialog->resizable(NULL);
	_dialog->callback((Fl_Callback *)close_cb, this);
	_dialog->set_modal();
	// Initialize dialog's children
	_body->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
	_progress->minimum(0.0f);
	_progress->maximum(1.0f);
	_progress->value(0.0f);
	_cancel_button->tooltip("Cancel (Enter)");
	_cancel_button->callback((Fl_Callback *)cancel_cb, this);
	Fl_Group::current(prev_current);
}

void Progress_Dialog::refresh() {
	_canceled = false;
	// Refresh widget positions and sizes
	int dy = 10;
	int w = 300;
	int ww = w - 20;
	_body->resize(10, 10, ww, 21);
	dy += _body->h() + 10;
	_progress->resize(10, dy, ww, 15);
	dy += _progress->h() + 16;
	_cancel_button->resize(w-90, dy, 80, 22);
	dy += _cancel_button->h() + 10;
	_dialog->size_range(w, dy, w, dy);
	_dialog->size(w, dy);
	_dialog->redraw();
}

// Show progress on taskbar button in Windows 7/8
// <http://stackoverflow.com/questions/15001638/windows-7-taskbar-state-with-minimal-code>
// <http://www.codeproject.com/Articles/80082/Windows-7-How-to-display-progress-bar-on-taskbar-i>
bool Progress_Dialog::initialize_taskbar() {
	if (_taskbar_list3) { return true; }
	if (_taskbar_failed) { return false; }
	// False positive "C6031: Return value ignored" errors with Visual Studio 2013 code analysis
	CoInitialize(NULL);
	CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_ALL, IID_ITaskbarList3, (void **)&_taskbar_list3);
	if (_taskbar_list3) { return true; }
	_taskbar_failed = true;
	CoUninitialize();
	return false;
}

void Progress_Dialog::show(const Fl_Widget *p) {
	fl_cursor(FL_CURSOR_WAIT);
	initialize();
	refresh();
	int x = p->x() + (p->w() - _dialog->w()) / 2;
	int y = p->y() + (p->h() - _dialog->h()) / 2;
	_dialog->position(x, y);
	_dialog->show();
	if (initialize_taskbar()) {
		_taskbar_list3->SetProgressState(fl_xid(_top_window), TBPF_NORMAL);
	}
}

void Progress_Dialog::hide() {
	_dialog->hide();
	fl_cursor(FL_CURSOR_DEFAULT);
	HWND top_hwnd = fl_xid(_top_window);
	if (initialize_taskbar()) {
		_taskbar_list3->SetProgressState(top_hwnd, TBPF_NOPROGRESS);
	}
	// Flash taskbar button
	// <http://blogs.msdn.com/b/oldnewthing/archive/2008/05/12/8490184.aspx>
	HWND fgw = GetForegroundWindow();
	if (fgw != top_hwnd && fgw != fl_xid(_dialog)) {
		FLASHWINFO fwi;
		fwi.cbSize = sizeof(fwi);
		fwi.hwnd = top_hwnd;
		fwi.dwFlags = FLASHW_ALL;
		fwi.dwTimeout = 0;
		fwi.uCount = 3;
		FlashWindowEx(&fwi);
	}
}

void Progress_Dialog::progress(float p) {
	_progress->value(p);
	if (initialize_taskbar()) {
		ULONGLONG c = (ULONGLONG)(p * PROGRESS_STEPS);
		if (c > PROGRESS_STEPS) { c = PROGRESS_STEPS; }
		_taskbar_list3->SetProgressValue(fl_xid(_top_window), c, PROGRESS_STEPS);
	}
}

void Progress_Dialog::close_cb(Fl_Widget *w, Progress_Dialog *pd) {
	// Override default behavior of Esc to close window
	if (Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Escape) {
		cancel_cb(w, pd);
		return;
	}
	pd->hide();
}

void Progress_Dialog::cancel_cb(Fl_Widget *, Progress_Dialog *pd) { pd->_canceled = true; }
