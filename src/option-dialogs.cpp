#include <cctype>
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <cstdlib>

#pragma warning(push, 0)
#include <FL/Enumerations.H>
#include <FL/Fl_Double_Window.H>
#pragma warning(pop)

#include "preferences.h"
#include "config.h"
#include "themes.h"
#include "widgets.h"
#include "utils.h"
#include "option-dialogs.h"
#include "parse-asm.h"

Option_Dialog::Option_Dialog(int w, const char *t) : _width(w), _title(t), _canceled(false),
	_dialog(NULL), _content(NULL), _ok_button(NULL), _cancel_button(NULL) {}

Option_Dialog::~Option_Dialog() {
	delete _dialog;
	delete _content;
	delete _ok_button;
	delete _cancel_button;
}

void Option_Dialog::initialize() {
	if (_dialog) { return; }
	Fl_Group *prev_current = Fl_Group::current();
	Fl_Group::current(NULL);
	// Populate dialog
	_dialog = new Fl_Double_Window(0, 0, 0, 0, _title);
	_content = new Fl_Group(0, 0, 0, 0);
	_content->begin();
	initialize_content();
	_content->end();
	_dialog->begin();
	_ok_button = new Default_Button(0, 0, 0, 0, "OK");
	_cancel_button = new OS_Button(0, 0, 0, 0, "Cancel");
	_dialog->end();
	// Initialize dialog
	_dialog->resizable(NULL);
	_dialog->callback((Fl_Callback *)cancel_cb, this);
	_dialog->set_modal();
	// Initialize dialog's children
	_ok_button->tooltip("OK (Enter)");
	_ok_button->callback((Fl_Callback *)close_cb, this);
	_cancel_button->shortcut(FL_Escape);
	_cancel_button->tooltip("Cancel (Esc)");
	_cancel_button->callback((Fl_Callback *)cancel_cb, this);
	Fl_Group::current(prev_current);
}

void Option_Dialog::refresh() {
	_canceled = false;
	_dialog->copy_label(_title);
	// Refresh widget positions and sizes
	fl_font(OS_FONT, OS_FONT_SIZE);
	int dy = 10;
	dy += refresh_content(_width - 20, dy) + 16;
#ifdef _WIN32
	_ok_button->resize(_width - 184, dy, 80, 22);
	_cancel_button->resize(_width - 90, dy, 80, 22);
#else
	_cancel_button->resize(_width - 184, dy, 80, 22);
	_ok_button->resize(_width - 90, dy, 80, 22);
#endif
	dy += _cancel_button->h() + 10;
	_dialog->size_range(_width, dy, _width, dy);
	_dialog->size(_width, dy);
	_dialog->redraw();
}

void Option_Dialog::show(const Fl_Widget *p) {
	initialize();
	refresh();
	int x = p->x() + (p->w() - _dialog->w()) / 2;
	int y = p->y() + (p->h() - _dialog->h()) / 2;
	_dialog->position(x, y);
	_ok_button->take_focus();
	_dialog->show();
	while (_dialog->shown()) { Fl::wait(); }
}

void Option_Dialog::close_cb(Fl_Widget *, Option_Dialog *od) {
	od->_dialog->hide();
}

void Option_Dialog::cancel_cb(Fl_Widget *, Option_Dialog *od) {
	od->_canceled = true;
	od->_dialog->hide();
}

Map_Options_Dialog::Map_Options_Dialog(const char *t) : Option_Dialog(300, t), _max_tileset_name_length(0), _max_roof_name_length(0),
	_map_header(NULL), _map_size(NULL), _map_width(NULL), _map_height(NULL), _map_sizes(NULL), _tileset(NULL), _roof(NULL),
	_valid_sizes(), _original_names() {}

Map_Options_Dialog::~Map_Options_Dialog() {
	delete _map_header;
	delete _map_size;
	delete _map_width;
	delete _map_height;
	delete _map_sizes;
	delete _tileset;
	delete _roof;
}

const char *Map_Options_Dialog::tileset(void) const {
	if (!_tileset->mvalue()) { return NULL; }
	const char *t = _tileset->mvalue()->label();
	return original_name(t);
}

const char *Map_Options_Dialog::roof(void) const {
	if (!_roof->mvalue() || _roof->value() == 0) { return NULL; }
	return _roof->mvalue()->label();
}

const char *Map_Options_Dialog::original_name(const char *pretty_name) const {
	Dictionary::const_iterator it = _original_names.find(pretty_name);
	if (it == _original_names.end()) { return pretty_name; }
	return it->second.c_str();
}

inline static bool isupperordigit(int c) { return isupper(c) || isdigit(c); }
inline static int toconstant(int c) { return isalnum(c) ? toupper(c) : '_'; }

static void guess_map_constant(const char *name, char *constant) {
	char prev = (char)toconstant(*name);
	*constant++ = (char)toupper(*name++);

	const char *p = strchr(name, '.');
	size_t n = p ? p - name : strlen(name); // ignore extension and attribute data
	for (size_t i = 0; i < n; i++) {
		char c = *name;
		if ((islower((unsigned char)prev) && isupperordigit(c)) || // ...zA... -> ...Z_A...
			(i < n - 1 && isupperordigit(prev) && isupper((unsigned char)c) && islower((unsigned char)*(name+1)))) { // ...ZAb... -> ...Z_AB...
			*constant++ = '_';
		}
		prev = c;
		*constant++ = (char)toconstant(*name++);
	}
	*constant++ = ',';
	*constant = '\0';
}

bool Map_Options_Dialog::guess_map_size(const char *filename, const char *directory, Map_Attributes &attrs) {
	_map_sizes->clear();
	_valid_sizes.clear();

	if (!filename) {
		_map_header->label(NEW_MAP_NAME ":");
		_map_size->label(NULL);
		return false;
	}

	const char *name = fl_filename_name(filename);
	char buffer[FL_PATH_MAX] = {};
	strcpy(buffer, name);
	strcat(buffer, ":");
	_map_header->copy_label(buffer);

	size_t fs = file_size(filename);
	if (ends_with(filename, ".MAP")) {
		Parsed_Asm data(filename);
		fs = data.size();
	}
	sprintf(buffer, "(%zu B)", fs);
	_map_size->copy_label(buffer);
	add_valid_sizes(fs);

	std::cmatch cm;
	std::regex rx(".+\\.([0-9]+)x([0-9]+)(?:\\.[A-Za-z0-9_-]+)?\\.[Bb][Ll][Kk]");
	std::regex_match(filename, cm, rx);
	size_t n = cm.size();
	if (n == 3) {
		std::string sw(cm[1]), sh(cm[2]);
		int w = std::stoi(sw), h = std::stoi(sh);
		int i = add_map_size(w, h);
		select_map_size(i);
		return true;
	}

	char map_constants[FL_PATH_MAX] = {};
	Config::map_constants_path(map_constants, directory);

	std::ifstream ifs(map_constants);
	if (!ifs.good()) { return false; }

	char constant[FL_PATH_MAX] = {};
	guess_map_constant(name, constant);

	attrs.group = 0;
	while (ifs.good()) {
		bool w_x_h = false;
		std::string line;
		std::getline(ifs, line);
		trim(line);
		if (starts_with(line, "newgroup") && line[strlen("newgroup")] != ':') {
			attrs.group++;
			continue;
		}
		else if (starts_with(line, "map_const")) {
			// "map_const": pokecrystal
			line.erase(0, strlen("map_const") + 1); // include next whitespace character
			w_x_h = true;
		}
		else if (starts_with(line, "mapgroup")) {
			// "mapgroup": pokecrystal pre-2018
			line.erase(0, strlen("mapgroup") + 1); // include next whitespace character
		}
		else if (starts_with(line, "mapconst")) {
			// "mapconst": pokered
			line.erase(0, strlen("mapconst") + 1); // include next whitespace character
		}
		else {
			continue;
		}
		if (!starts_with(line, constant)) { continue; }
		line.erase(0, strlen(constant));
		std::istringstream lss(line);
		int w, h;
		char comma;
		if (w_x_h) {
			lss >> w >> comma >> h;
		}
		else {
			lss >> h >> comma >> w;
		}
		if (valid_size(w, h)) {
			int i = add_map_size(w, h);
			select_map_size(i);
			return true;
		}

		return false;
	}

	return false;
}

std::string Map_Options_Dialog::guess_map_tileset(const char *filename, const char *directory, Map_Attributes &attrs) {
	if (!filename) { return ""; }

	std::cmatch cm;
	std::regex rx(".+\\.([A-Za-z0-9_-]+)\\.[Bb][Ll][Kk]");
	std::regex_match(filename, cm, rx);
	size_t n = cm.size();
	if (n == 2) {
		return cm[1].str();
	}

	char map_name[FL_PATH_MAX] = {};
	remove_dot_ext(filename, map_name);

	char map_headers[FL_PATH_MAX] = {};
	bool map_headers_exist = Config::map_headers_path(map_headers, directory);

	if (map_headers_exist) {
		std::ifstream ifs(map_headers);
		if (!ifs.good()) { return ""; }

		while (ifs.good()) {
			std::string line;
			std::getline(ifs, line);
			remove_comment(line);
			trim(line);

			std::istringstream lss(line);

			std::string macro;
			lss >> macro;
			if (macro != "map_header" && macro != "map") { continue; }

			std::string map_label;
			std::getline(lss, map_label, ',');
			trim(map_label);
			if (map_label != map_name) { continue; }

			std::string tileset_name;
			std::getline(lss, tileset_name, ',');
			trim(tileset_name);
			if (starts_with(tileset_name, "TILESET_")) {
				tileset_name.erase(0, strlen("TILESET_"));
				lowercase(tileset_name);
			}
			else if (starts_with(tileset_name, "$")) {
				tileset_name.erase(0, 1);
				int ti = std::stoi(tileset_name, NULL, 16);
				char tileset_num[16] = {};
				sprintf(tileset_num, "%02d", ti);
				tileset_name = tileset_num;
			}
			else if (std::all_of(tileset_name.begin(), tileset_name.end(), isdigit)) {
				if (tileset_name.length() == 1) {
					tileset_name = "0" + tileset_name;
				}
			}
			else {
				tileset_name.erase();
			}

			std::string environment;
			std::getline(lss, environment, ',');
			trim(environment);
			attrs.environment = environment;

			std::string landmark;
			std::getline(lss, landmark, ',');
			trim(landmark);
			lowercase(landmark);
			attrs.landmark = landmark;

			std::string skip_token;
			std::getline(lss, skip_token, ','); // music
			std::getline(lss, skip_token, ','); // phone service flag

			std::string palette;
			std::getline(lss, palette, ',');
			trim(palette);
			attrs.palette = palette;

			return tileset_name;
		}
	}
	else {
		char map_header[FL_PATH_MAX] = {};
		Config::map_header_path(map_header, directory, map_name);

		std::ifstream ifs(map_header);
		if (!ifs.good()) { return ""; }

		while (ifs.good()) {
			std::string line;
			std::getline(ifs, line);
			remove_comment(line);
			trim(line);

			std::istringstream lss(line);

			std::string macro;
			lss >> macro;
			if (macro == "map_header") {
				std::string skip_token;
				std::getline(lss, skip_token, ','); // map name
				std::getline(lss, skip_token, ','); // map ID

				std::string tileset_name;
				std::getline(lss, tileset_name, ',');
				trim(tileset_name);
				lowercase(tileset_name);
				return tileset_name;
			}
			else if (macro == "db") {
				std::string tileset_name;
				lss >> tileset_name;
				lowercase(tileset_name);
				return tileset_name;
			}
		}
	}

	return "";
}

void Map_Options_Dialog::guess_tileset_names(const char *directory, Dictionary &pretty_names, Dictionary &guessable_names) {
	char tileset_constants[FL_PATH_MAX] = {};
	Config::tileset_constants_path(tileset_constants, directory);

	std::ifstream ifs(tileset_constants);
	if (!ifs.good()) { return; }

	int id = 1;
	char original[16] = {};
	while (ifs.good()) {
		std::string line;
		std::getline(ifs, line);
		std::istringstream lss(line);
		std::string token;
		lss >> token;
		if (token != "const") { continue; }
		lss >> token;
		if (starts_with(token, "TILESET_")) { token.erase(0, strlen("TILESET_")); }
		sprintf(original, "%02d", id++);
		std::string pretty = original + (": " + token);
		pretty_names[original] = pretty;
		std::string guessable = token;
		lowercase(guessable);
		guessable_names[original] = guessable;
	}

	return;
}

std::string Map_Options_Dialog::add_tileset(const char *t, int ext_len, const Dictionary &pretty_names) {
	std::string v(t);
	v.erase(v.size() - ext_len, ext_len);
	std::string p(v);

	if (p.length() == 2 && isdigit((unsigned char)p[0]) && isdigit((unsigned char)p[1])) {
		Dictionary::const_iterator it = pretty_names.find(p);
		if (it != pretty_names.end()) {
			p = it->second;
			_original_names[p] = it->first;
		}
	}

	if (_tileset->find_index(p.c_str()) == -1) {
		_tileset->add(p.c_str());
		int m = text_width(p.c_str(), 6);
		_max_tileset_name_length = MAX(m, _max_tileset_name_length);
	}

	return v;
}

void Map_Options_Dialog::add_roof(const char *r, int ext_len) {
	std::string v(r);
	v.erase(v.size() - ext_len, ext_len);

	if (_roof->find_index(v.c_str()) == -1) {
		_roof->add(v.c_str());
		int m = text_width(v.c_str(), 6);
		_max_roof_name_length = MAX(m, _max_roof_name_length);
	}
}

bool Map_Options_Dialog::limit_blk_options(const char *filename, const char *directory, Map_Attributes &attrs) {
	initialize();

	// Initialize map size
	if (!guess_map_size(filename, directory, attrs) && _valid_sizes.empty()) {
		_map_width->value(1);
		_map_height->value(1);
	}

	// Initialize tileset choices

	_tileset->clear();
	_max_tileset_name_length = 0;

	char tileset_directory[FL_PATH_MAX] = {};
	Config::gfx_tileset_dir(tileset_directory, directory);

	dirent **list;
	int n = fl_filename_list(tileset_directory, &list);
	if (n < 0) { return false; }

	std::string guessed_tileset_name = guess_map_tileset(filename, directory, attrs);
	Dictionary pretty_names, guessable_names;
	guess_tileset_names(directory, pretty_names, guessable_names);
	int v = 0;
	for (int i = 0; i < n; i++) {
		const char *name = list[i]->d_name;
		if (ends_with(name, ".colored.png")) { continue; } // ignore utils/metatiles.py renders
		int ext_len = ends_with(name, ".2bpp.lz") ? 8 :
			          ends_with(name, ".2bpp.unique.lz") ? 15 : // for Red++ 3.0's generic+unique tilesets
			          ends_with(name, ".2bpp") ? 5 :
			          ends_with(name, ".CHR") ? 4 :
			          ends_with(name, ".png") ? 4 : 0;
		if (ext_len) {
			std::string guessable_name(add_tileset(name, ext_len, pretty_names));
			if (guessed_tileset_name == guessable_name) { v = _tileset->size() - 2; } // ignore terminating NULL
			Dictionary::const_iterator it = guessable_names.find(guessable_name);
			if (it != guessable_names.end()) {
				guessable_name = it->second;
			}
			if (guessed_tileset_name == guessable_name) { v = _tileset->size() - 2; } // ignore terminating NULL
		}
	}
	_tileset->value(v);

	fl_filename_free_list(&list, n);

	// Initialize roof choices

	_roof->clear();
	_roof->add("(none)");
	_roof->value(0);
	_max_roof_name_length = text_width("(none)", 6);

	char roof_directory[FL_PATH_MAX] = {};
	Config::gfx_roof_dir(roof_directory, directory);

	n = fl_filename_list(roof_directory, &list);
	if (n >= 0) {
		for (int i = 0; i < n; i++) {
			const char *name = list[i]->d_name;
			int ext_len = ends_with(name, ".2bpp.lz") ? 8 :
			              ends_with(name, ".2bpp") ? 5 :
			              ends_with(name, ".CHR") ? 4 :
			              ends_with(name, ".png") ? 4 : 0;
			if (ext_len) {
				add_roof(name, ext_len);
			}
		}
		fl_filename_free_list(&list, n);
	}

	if (num_roofs() > 0) {
		_roof->activate();
	}
	else {
		_roof->deactivate();
	}

	return true;
}

int Map_Options_Dialog::add_map_size(int w, int h) {
	std::pair<int, int> s(w, h);
	std::vector<std::pair<int, int>>::iterator it = std::find(_valid_sizes.begin(), _valid_sizes.end(), s);
	if (it == _valid_sizes.end()) {
		_valid_sizes.push_back(s);
		char buffer[128] = {};
		sprintf(buffer, "%d x %d", w, h);
		_map_sizes->add(buffer);
		return (int)_valid_sizes.size() - 1;
	}
	return std::distance(_valid_sizes.begin(), it);
}

void Map_Options_Dialog::add_valid_sizes(size_t n) {
	int s = (int)n;
	for (int w = s; w >= 1; w--) {
		int h = s / w;
		if (valid_size(w, h) && w * h == s) {
			add_map_size(w, h);
		}
	}
	if (!_valid_sizes.empty()) {
		select_map_size(((int)_valid_sizes.size() - 1) / 2);
	}
}

void Map_Options_Dialog::initialize_content() {
	// Populate content group
	_map_header = new Label(0, 0, 0, 0);
	_map_size = new Label(0, 0, 0, 0);
	_map_width = new OS_Spinner(0, 0, 0, 0, "Width:");
	_map_height = new OS_Spinner(0, 0, 0, 0, "Height:");
	_map_sizes = new Dropdown(0, 0, 0, 0, "Size:");
	_tileset = new Dropdown(0, 0, 0, 0, "Tileset:");
	_roof = new Dropdown(0, 0, 0, 0, "Roof:");
	// Initialize content group's children
	_map_width->align(FL_ALIGN_LEFT);
	_map_width->range(1, 255);
	_map_height->align(FL_ALIGN_LEFT);
	_map_height->range(1, 255);
	_map_sizes->align(FL_ALIGN_LEFT);
	_map_sizes->callback((Fl_Callback *)select_valid_size_cb, this);
	_tileset->align(FL_ALIGN_LEFT);
	_roof->align(FL_ALIGN_LEFT);
	// Initialize data
	_valid_sizes.clear();
	_original_names.clear();
}

int Map_Options_Dialog::refresh_content(int ww, int dy) {
	int wgt_w = 0, wgt_h = 22, win_m = 10, wgt_m = 4;
	int ch = wgt_h * 4 + wgt_m * 3;
	_content->resize(win_m, dy, ww, ch);

	_map_header->resize(win_m, dy, ww, wgt_h);
	dy += _map_header->h() + wgt_m;

	int wgt_off;
	if (_valid_sizes.empty()) {
		_map_width->set_visible();
		_map_height->set_visible();
		_map_sizes->clear_visible();

		wgt_off = win_m + MAX(text_width(_map_width->label(), 2), text_width(_tileset->label(), 2));

		wgt_w = text_width("999", 2) + wgt_h;
		_map_width->resize(wgt_off, dy, wgt_w, wgt_h);
		_map_height->resize(_map_width->x() + _map_width->w() + 10 + text_width("Height:"), dy, wgt_w, wgt_h);

		int mso = _map_height->x() + _map_height->w();
		_map_size->resize(mso + 10, dy, ww - mso, wgt_h);
		dy += _map_height->h() + wgt_m;
	}
	else {
		_map_width->clear_visible();
		_map_height->clear_visible();
		_map_sizes->set_visible();

		wgt_off = win_m + MAX(text_width(_map_sizes->label(), 2), text_width(_tileset->label(), 2));

		wgt_w = text_width("9999 x 9999", 2) + wgt_h;
		_map_sizes->resize(wgt_off, dy, wgt_w, wgt_h);

		int mso = _map_sizes->x() + _map_sizes->w();
		_map_size->resize(mso + 10, dy, ww - mso, wgt_h);
		dy += _map_sizes->h() + wgt_m;
	}


	wgt_w = _max_tileset_name_length + wgt_h;
	_tileset->resize(wgt_off, dy, wgt_w, wgt_h);
	dy += _tileset->h() + wgt_m;

	wgt_w = _max_roof_name_length + wgt_h;
	_roof->resize(wgt_off, dy, wgt_w, wgt_h);

	return ch;
}

void Map_Options_Dialog::select_valid_size_cb(Dropdown *, Map_Options_Dialog *md) {
	int i = md->_map_sizes->value();
	std::pair<int, int> wh = md->_valid_sizes[i];
	md->_map_width->value(wh.first);
	md->_map_height->value(wh.second);
}

class Anchor_Button : public OS_Button {
private:
	int _anchor;
public:
	Anchor_Button(int a) : OS_Button(0, 0, 0, 0), _anchor(a) {
		type(FL_RADIO_BUTTON);
		align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_CLIP);
	}
	inline int anchor(void) const { return _anchor; }
};

Resize_Dialog::Resize_Dialog(const char *t) : Option_Dialog(220, t), _map_width(NULL), _map_height(NULL), _anchor_buttons() {}

Resize_Dialog::~Resize_Dialog() {
	delete _map_width;
	delete _map_height;
	for (int i = 0; i < 9; i++) { delete _anchor_buttons[i]; }
}

Resize_Dialog::Hor_Align Resize_Dialog::horizontal_anchor() const {
	if (_anchor_buttons[0]->value() || _anchor_buttons[3]->value() || _anchor_buttons[6]->value()) { return Hor_Align::LEFT; }
	if (_anchor_buttons[2]->value() || _anchor_buttons[5]->value() || _anchor_buttons[8]->value()) { return Hor_Align::RIGHT; }
	return Hor_Align::CENTER;
}

Resize_Dialog::Vert_Align Resize_Dialog::vertical_anchor() const {
	if (_anchor_buttons[0]->value() || _anchor_buttons[1]->value() || _anchor_buttons[2]->value()) { return Vert_Align::TOP; }
	if (_anchor_buttons[6]->value() || _anchor_buttons[7]->value() || _anchor_buttons[8]->value()) { return Vert_Align::BOTTOM; }
	return Vert_Align::MIDDLE;
}

int Resize_Dialog::anchor() const {
	for (int i = 0; i < 9; i++) {
		if (_anchor_buttons[i]->value()) { return i; }
	}
	return 4;
}

void Resize_Dialog::anchor(int a) {
	_anchor_buttons[a]->do_callback();
}

void Resize_Dialog::anchor_label(int x, int y, const char *l) {
	if (0 <= x && x < 3 && 0 <= y && y < 3) {
		_anchor_buttons[y * 3 + x]->label(l);
	}
}

void Resize_Dialog::initialize_content() {
	// Populate content group
	_map_width = new Default_Spinner(0, 0, 0, 0, "Width:");
	_map_height = new Default_Spinner(0, 0, 0, 0, "Height:");
	for (int i = 0; i < 9; i++) {
		Anchor_Button *ab = new Anchor_Button(i);
		ab->callback((Fl_Callback *)anchor_button_cb, this);
		_anchor_buttons[i] = ab;
	}
	// Initialize content group's children
	_map_width->align(FL_ALIGN_LEFT);
	_map_width->range(1, 255);
	_map_height->align(FL_ALIGN_LEFT);
	_map_height->range(1, 255);
	anchor(Preferences::get("resize-anchor", 4));
}

int Resize_Dialog::refresh_content(int ww, int dy) {
	int wgt_w = 0, wgt_h = 22, win_m = 10, wgt_m = 4;
	int ch = (wgt_h + wgt_m) * 2 + wgt_h;
	_content->resize(win_m, dy, ww, ch);

	int wgt_off = win_m + MAX(text_width(_map_width->label(), 2), text_width(_map_height->label(), 2));

	wgt_w = text_width("999", 2) + wgt_h;
	_map_width->resize(wgt_off, dy, wgt_w, wgt_h);
	_map_height->resize(wgt_off, _map_width->y() + _map_width->h() + wgt_m, wgt_w, wgt_h);
	wgt_off += wgt_w + 20;

	wgt_w = wgt_h = 24;
	wgt_m = 2;
	for (int ay = 0; ay < 3; ay++) {
		for (int ax = 0; ax < 3; ax++) {
			_anchor_buttons[ay*3+ax]->resize(wgt_off + (wgt_w + wgt_m) * ax, dy, wgt_w, wgt_h);
		}
		dy += wgt_h + wgt_m;
	}

	return ch;
}

void Resize_Dialog::anchor_button_cb(Anchor_Button *ab, Resize_Dialog *rd) {
	ab->setonly();
	for (int i = 0; i < 9; i++) {
		rd->_anchor_buttons[i]->label(NULL);
	}
	int a = ab->anchor();
	int y = a / 3, x = a % 3;
	rd->anchor_label(x - 1, y - 1, "@7>"); // top-left
	rd->anchor_label(x,     y - 1, "@8>"); // top
	rd->anchor_label(x + 1, y - 1, "@9>"); // top-right
	rd->anchor_label(x - 1,  y,    "@4>"); // left
	rd->anchor_label(x,      y,    "@-2square"); // center
	rd->anchor_label(x + 1,  y,    "@>");  // right
	rd->anchor_label(x - 1, y + 1, "@1>"); // bottom-left
	rd->anchor_label(x,     y + 1, "@2>"); // bottom
	rd->anchor_label(x + 1, y + 1, "@3>"); // bottom-right
	rd->_dialog->redraw();
}

Add_Sub_Dialog::Add_Sub_Dialog(const char *t) : Option_Dialog(194, t), _num_metatiles(NULL) {}

Add_Sub_Dialog::~Add_Sub_Dialog() {
	delete _num_metatiles;
}

void Add_Sub_Dialog::initialize_content() {
	// Populate content group
	_num_metatiles = new Default_Spinner(0, 0, 0, 0, "Blocks:");
	// Initialize content group's children
	_num_metatiles->align(FL_ALIGN_LEFT);
	_num_metatiles->range(1, 256);
}

int Add_Sub_Dialog::refresh_content(int ww, int dy) {
	int wgt_h = 22, win_m = 10;
	_content->resize(win_m, dy, ww, wgt_h);

	int wgt_off = win_m + text_width(_num_metatiles->label(), 2);
	int wgt_w = text_width("999", 2) + wgt_h;
	_num_metatiles->resize(wgt_off, dy, wgt_w, wgt_h);

	return wgt_h;
}

Overworld_Map_Size_Dialog::Overworld_Map_Size_Dialog(const char *t) : Option_Dialog(380, t), _description(NULL),
	_overworld_map_size(NULL) {}

Overworld_Map_Size_Dialog::~Overworld_Map_Size_Dialog() {
	delete _description;
	delete _overworld_map_size;
}

void Overworld_Map_Size_Dialog::initialize_content() {
	// Populate content group
	_description = new Label(0, 0, 0, 0,
		"The overworld map block buffer in WRAM stores the entire\n"
		"current map, padded by 3 blocks on each side.\n\n"
		"Its standard label in wram.asm is wOverworldMapBlocks,\n"
		"wOverworldMap, or OverworldMap.\n\n"
		"Its default size is 1300 bytes, so every map requires that\n"
		"(width + 6) \xc3\x97 (height + 6) \xe2\x89\xa4 1300."
	);
	_overworld_map_size = new Default_Spinner(0, 0, 0, 0, "ds:");
	// Initialize content group's children
	_overworld_map_size->align(FL_ALIGN_LEFT);
	_overworld_map_size->range(1, 4096);
	_overworld_map_size->labelfont(FL_COURIER);
}

int Overworld_Map_Size_Dialog::refresh_content(int ww, int dy) {
	int wgt_h = 22, win_m = 10;

	int ew = 0, eh = 0;
	fl_measure(_description->label(), ew, eh, 0);
	eh += wgt_h - _description->labelsize();

	int ch = eh + wgt_h + wgt_h;
	_content->resize(win_m, dy, ww, ch);

	_description->resize(win_m, dy, ww, eh);
	dy += eh + wgt_h;

	Fl_Font f = fl_font();
	Fl_Fontsize s = fl_size();
	fl_font(_overworld_map_size->labelfont(), _overworld_map_size->labelsize());
	int wgt_off = win_m + text_width(_overworld_map_size->label(), 2);
	fl_font(f, s);
	int wgt_w = text_width("9999", 2) + wgt_h;
	_overworld_map_size->resize(wgt_off, dy, wgt_w, wgt_h);

	return ch;
}

Tileset_Options_Dialog::Tileset_Options_Dialog(const char *t, Map_Options_Dialog *mod) : Option_Dialog(280, t),
	_tileset(NULL), _map_options_dialog(mod) {}

Tileset_Options_Dialog::~Tileset_Options_Dialog() {
	delete _tileset;
}

bool Tileset_Options_Dialog::limit_tileset_options(const char *old_tileset_name) {
	if (!_map_options_dialog) { return false; }
	initialize();
	// Copy _map_options_dialog's tileset choices
	_tileset->clear();
	int n = _map_options_dialog->_tileset->size() - 1; // ignore terminating NULL
	int v = 0;
	for (int i = 0; i < n; i++) {
		const Fl_Menu_Item &item = _map_options_dialog->_tileset->menu()[i];
		const char *name = item.label();
		const char *original_name = _map_options_dialog->original_name(name);
		if (!strcmp(old_tileset_name, original_name)) { v = i; }
		_tileset->add(name);
	}
	_tileset->value(v);
	return true;
}

const char *Tileset_Options_Dialog::tileset(void) const {
	if (!_tileset->mvalue()) { return NULL; }
	const char *t = _tileset->mvalue()->label();
	Dictionary::const_iterator it = _map_options_dialog->_original_names.find(t);
	if (it == _map_options_dialog->_original_names.end()) { return t; }
	return it->second.c_str();
}

void Tileset_Options_Dialog::initialize_content() {
	// Populate content group
	_tileset = new Dropdown(0, 0, 0, 0, "Tileset:");
	// Initialize content group's children
	_tileset->align(FL_ALIGN_LEFT);
}

int Tileset_Options_Dialog::refresh_content(int ww, int dy) {
	int wgt_w = 0, wgt_h = 22, win_m = 10;
	int ch = wgt_h;
	_content->resize(win_m, dy, ww, ch);

	int wgt_off = win_m + text_width(_tileset->label(), 2);

	wgt_w = _map_options_dialog->_max_tileset_name_length + wgt_h;
	_tileset->resize(wgt_off, dy, wgt_w, wgt_h);

	return ch;
}

Roof_Options_Dialog::Roof_Options_Dialog(const char *t, Map_Options_Dialog *mod) : Option_Dialog(280, t),
	_roof(NULL), _map_options_dialog(mod) {}

Roof_Options_Dialog::~Roof_Options_Dialog() {
	delete _roof;
}

bool Roof_Options_Dialog::limit_roof_options(const char *old_roof_name) {
	if (!_map_options_dialog) { return false; }
	initialize();
	// Copy _map_options_dialog's roof choices
	_roof->clear();
	int n = _map_options_dialog->_roof->size() - 1; // ignore terminating NULL
	int v = 0;
	for (int i = 0; i < n; i++) {
		const Fl_Menu_Item &item = _map_options_dialog->_roof->menu()[i];
		const char *name = item.label();
		if (old_roof_name && !strcmp(old_roof_name, name)) { v = i; }
		_roof->add(name);
	}
	_roof->value(v);
	return true;
}

const char *Roof_Options_Dialog::roof(void) const {
	if (!_roof->mvalue() || !_roof->value()) { return NULL; }
	return _roof->mvalue()->label();
}

void Roof_Options_Dialog::initialize_content() {
	// Populate content group
	_roof = new Dropdown(0, 0, 0, 0, "Roof:");
	// Initialize content group's children
	_roof->align(FL_ALIGN_LEFT);
}

int Roof_Options_Dialog::refresh_content(int ww, int dy) {
	int wgt_w = 0, wgt_h = 22, win_m = 10;
	int ch = wgt_h;
	_content->resize(win_m, dy, ww, ch);

	int wgt_off = win_m + text_width(_roof->label(), 2);

	wgt_w = _map_options_dialog->_max_roof_name_length + wgt_h;
	_roof->resize(wgt_off, dy, wgt_w, wgt_h);

	return ch;
}

Event_Options_Dialog::Event_Options_Dialog(const char *t) : Option_Dialog(300, t), _macro_heading(NULL),
	_line_heading(NULL), _prefix(NULL), _suffix(NULL), _event_x(NULL), _event_y(NULL),
	_hex_event_x(NULL), _hex_event_y(NULL) {}

Event_Options_Dialog::~Event_Options_Dialog() {
	delete _macro_heading;
	delete _line_heading;
	delete _prefix;
	delete _suffix;
	delete _event_x;
	delete _event_y;
}

void Event_Options_Dialog::use_event(const Event *e) {
	initialize();
	char buffer[512];
#if defined(_MSC_VER) && _MSC_VER < 1900
	_snprintf_s(buffer, sizeof(buffer), "%s:", e->_macro.c_str());
#else
	snprintf(buffer, sizeof(buffer), "%s:", e->_macro.c_str());
#endif
	_macro_heading->copy_label(buffer);

	sprintf(buffer, "Line: %zu", e->_line);
	_line_heading->copy_label(buffer);

	_prefix->value(e->_prefix.c_str());
	if (e->_prefixed) {
		_prefix->show();
	}
	else {
		_prefix->hide();
	}
	_prefix->position(0);

	_suffix->value(e->_suffix.c_str());
	if (e->_suffixed) {
		_suffix->show();
	}
	else {
		_suffix->hide();
	}
	_suffix->position(0);

	_event_x->default_value(e->_event_x);
	_hex_event_x->default_value(e->_event_x);
	if (e->_hex_coords) {
		_event_x->hide();
		_hex_event_x->show();
	}
	else {
		_event_x->show();
		_hex_event_x->hide();
	}

	_event_y->default_value(e->_event_y);
	_hex_event_y->default_value(e->_event_y);
	if (e->_hex_coords) {
		_event_y->hide();
		_hex_event_y->show();
	}
	else {
		_event_y->show();
		_hex_event_y->hide();
	}
}

void Event_Options_Dialog::update_event(Event *e) const {
	if (e->_prefixed) {
		e->_prefix = _prefix->value();
		trim(e->_prefix);
		std::replace(e->_prefix.begin(), e->_prefix.end(), '\n', ' ');
		if (!ends_with(e->_prefix, ",")) {
			e->_prefix += ",";
		}
	}
	if (e->_suffixed) {
		e->_suffix = _suffix->value();
		trim(e->_suffix);
		std::replace(e->_suffix.begin(), e->_suffix.end(), '\n', ' ');
		if (starts_with(e->_suffix, ";")) {
			e->_suffix = " " + e->_suffix;
		}
		else if (!starts_with(e->_suffix, ",")) {
			e->_suffix = ", " + e->_suffix;
		}
	}
	if (e->_hex_coords) {
		e->_event_x = (int16_t)_hex_event_x->value();
		e->_event_y = (int16_t)_hex_event_y->value();
	}
	else {
		e->_event_x = (int16_t)_event_x->value();
		e->_event_y = (int16_t)_event_y->value();
	}
	e->update_tooltip();
}

void Event_Options_Dialog::initialize_content() {
	// Populate content group
	_macro_heading = new Label(0, 0, 0, 0);
	_line_heading = new Label(0, 0, 0, 0);
	_prefix = new OS_Input(0, 0, 0, 0, "Prefix:");
	_suffix = new OS_Input(0, 0, 0, 0, "Suffix:");
	_event_x = new Default_Spinner(0, 0, 0, 0, "X:");
	_event_y = new Default_Spinner(0, 0, 0, 0, "Y:");
	_hex_event_x = new Default_Hex_Spinner(0, 0, 0, 0, "X:");
	_hex_event_y = new Default_Hex_Spinner(0, 0, 0, 0, "Y:");
	// Initialize content group's children
	_event_x->align(FL_ALIGN_LEFT);
	_event_x->range((double)MIN_EVENT_COORD, (double)MAX_EVENT_COORD);
	_event_y->align(FL_ALIGN_LEFT);
	_event_y->range((double)MIN_EVENT_COORD, (double)MAX_EVENT_COORD);
	_hex_event_x->align(FL_ALIGN_LEFT);
	_hex_event_x->range((int)MIN_EVENT_COORD, (int)MAX_EVENT_COORD);
	_hex_event_y->align(FL_ALIGN_LEFT);
	_hex_event_y->range((int)MIN_EVENT_COORD, (int)MAX_EVENT_COORD);
}

int Event_Options_Dialog::refresh_content(int ww, int dy) {
	int wgt_w = 0, wgt_h = 22, win_m = 10, wgt_m = 4;

	int wgt_off = text_width(_event_x->label(), 2);
	if (_prefix->visible()) {
		wgt_off = MAX(wgt_off, text_width(_prefix->label(), 2));
	}
	if (_suffix->visible()) {
		wgt_off = MAX(wgt_off, text_width(_suffix->label(), 2));
	}
	wgt_off += win_m;

	int ph = wgt_h, sh = wgt_h;
	if (_prefix->visible()) {
		int pvw = ww-wgt_off+win_m, pvh = 0;
		fl_measure(_prefix->value(), pvw, pvh, 0);
		ph = MAX(wgt_h, pvh + Fl::box_dh(_prefix->box()));
	}
	if (_suffix->visible()) {
		int svw = ww-wgt_off+win_m, svh = 0;
		fl_measure(_suffix->value(), svw, svh, 0);
		sh = MAX(wgt_h, svh + Fl::box_dh(_suffix->box()));
	}

	int ch = wgt_h + wgt_m + wgt_h;
	if (_prefix->visible()) {
		ch += ph + wgt_m;
	}
	if (_suffix->visible()) {
		ch += sh + wgt_m;
	}
	_content->resize(win_m, dy, ww, ch);

	wgt_w = text_width(_line_heading->label(), 6);
	_macro_heading->resize(win_m, dy, ww-wgt_w, wgt_h);
	_line_heading->resize(win_m+ww-wgt_w, dy, wgt_w, wgt_h);
	dy += wgt_h + wgt_m;

	if (_prefix->visible()) {
		if (ph > wgt_h) {
			_prefix->align(FL_ALIGN_LEFT_TOP | FL_ALIGN_CLIP);
			_prefix->type(FL_MULTILINE_INPUT_WRAP);
		}
		else {
			_prefix->align(FL_ALIGN_LEFT | FL_ALIGN_CLIP);
			_prefix->type(FL_NORMAL_INPUT);
		}
		_prefix->resize(wgt_off, dy, ww-wgt_off+win_m, ph);
		dy += ph + wgt_m;
	}

	if (_event_x->visible()) {
		int dx = text_width(_event_y->label(), 2) + win_m;
		wgt_w = text_width("999", 2) + wgt_h;
		_event_x->resize(wgt_off, dy, wgt_w, wgt_h);
		_event_y->resize(wgt_off+wgt_w+dx, dy, wgt_w, wgt_h);
	}
	else {
		int dx = text_width(_hex_event_y->label(), 2) + win_m;
		wgt_w = MAX(text_width("AA", 2), text_width("FF", 2)) + wgt_h;
		_hex_event_x->resize(wgt_off, dy, wgt_w, wgt_h);
		_hex_event_y->resize(wgt_off+wgt_w+dx, dy, wgt_w, wgt_h);
	}
	dy += wgt_h + wgt_m;

	if (_suffix->visible()) {
		if (sh > wgt_h) {
			_suffix->align(FL_ALIGN_LEFT_TOP | FL_ALIGN_CLIP);
			_suffix->type(FL_MULTILINE_INPUT_WRAP);
		}
		else {
			_suffix->align(FL_ALIGN_LEFT | FL_ALIGN_CLIP);
			_suffix->type(FL_NORMAL_INPUT);
		}
		_suffix->resize(wgt_off, dy, ww-wgt_off+win_m, sh);
	}

	return ch;
}

Print_Options_Dialog::Print_Options_Dialog(const char *t) : _title(t), _copied(false), _canceled(false), _dialog(NULL),
	_show_heading(NULL), _grid(NULL), _ids(NULL), _priority(NULL), _events(NULL), _export_button(NULL), _copy_button(NULL),
	_cancel_button(NULL) {}

Print_Options_Dialog::~Print_Options_Dialog() {
	delete _dialog;
	delete _show_heading;
	delete _grid;
	delete _ids;
	delete _priority;
	delete _events;
	delete _export_button;
	delete _copy_button;
	delete _cancel_button;
}

void Print_Options_Dialog::initialize() {
	if (_dialog) { return; }
	Fl_Group *prev_current = Fl_Group::current();
	Fl_Group::current(NULL);
	// Populate dialog
	_dialog = new Fl_Double_Window(0, 0, 0, 0, _title);
	_show_heading = new Label(0, 0, 0, 0, "Show:");
	_grid = new OS_Check_Button(0, 0, 0, 0, "Grid");
	_ids = new OS_Check_Button(0, 0, 0, 0, "IDs");
	_priority = new OS_Check_Button(0, 0, 0, 0, "Priority");
	_events = new OS_Check_Button(0, 0, 0, 0, "Events");
	_export_button = new Default_Button(0, 0, 0, 0, "Export...");
	_copy_button = new OS_Button(0, 0, 0, 0, "Copy");
	_cancel_button = new OS_Button(0, 0, 0, 0, "Cancel");
	_dialog->end();
	// Initialize dialog
	_dialog->resizable(NULL);
	_dialog->callback((Fl_Callback *)cancel_cb, this);
	_dialog->set_modal();
	// Initialize dialog's children
	_export_button->tooltip("Export (Enter)");
	_export_button->callback((Fl_Callback *)close_cb, this);
	_copy_button->shortcut(FL_COMMAND + 'c');
	_copy_button->tooltip("Copy (Ctrl+C)");
	_copy_button->callback((Fl_Callback *)copy_cb, this);
	_cancel_button->shortcut(FL_Escape);
	_cancel_button->tooltip("Cancel (Esc)");
	_cancel_button->callback((Fl_Callback *)cancel_cb, this);
	Fl_Group::current(prev_current);
}

void Print_Options_Dialog::refresh() {
	_copied = _canceled = false;
	_dialog->copy_label(_title);
	// Refresh widget positions and sizes
	fl_font(OS_FONT, OS_FONT_SIZE);
	int btn_w = 80, wgt_h = 22, win_m = 10, wgt_m = 4;
	int dx = win_m, dy = win_m;
	int wgt_w = text_width(_show_heading->label(), 4);
	_show_heading->resize(dx, dy, wgt_w, wgt_h);
	dx += _show_heading->w() + wgt_m;
	wgt_w = text_width(_grid->label(), 2) + wgt_h;
	_grid->resize(dx, dy, wgt_w, wgt_h);
	dx += _grid->w() + wgt_m;
	wgt_w = text_width(_ids->label(), 2) + wgt_h;
	_ids->resize(dx, dy, wgt_w, wgt_h);
	dx += _ids->w() + wgt_m;
	wgt_w = text_width(_priority->label(), 2) + wgt_h;
	_priority->resize(dx, dy, wgt_w, wgt_h);
	dx += _priority->w() + wgt_m;
	wgt_w = text_width(_events->label(), 2) + wgt_h;
	_events->resize(dx, dy, wgt_w, wgt_h);
	dx += _events->w() + win_m;
	if (dx < 288) { dx = 288; }
	dy += wgt_h + 16;
#ifdef _WIN32
	_export_button->resize(dx - 278, dy, btn_w, wgt_h);
	_copy_button->resize(dx - 184, dy, btn_w, wgt_h);
	_cancel_button->resize(dx - 90, dy, btn_w, wgt_h);
#else
	_cancel_button->resize(dx - 278, dy, btn_w, wgt_h);
	_copy_button->resize(dx - 184, dy, btn_w, wgt_h);
	_export_button->resize(dx - 90, dy, btn_w, wgt_h);
#endif
	dy += wgt_h + win_m;
	_dialog->size_range(dx, dy, dx, dy);
	_dialog->size(dx, dy);
	_dialog->redraw();
}

void Print_Options_Dialog::show(const Fl_Widget *p) {
	initialize();
	refresh();
	int x = p->x() + (p->w() - _dialog->w()) / 2;
	int y = p->y() + (p->h() - _dialog->h()) / 2;
	_dialog->position(x, y);
	_export_button->take_focus();
	_dialog->show();
	while (_dialog->shown()) { Fl::wait(); }
}

void Print_Options_Dialog::close_cb(Fl_Widget *, Print_Options_Dialog *pd) {
	pd->_dialog->hide();
}

void Print_Options_Dialog::copy_cb(Fl_Widget *, Print_Options_Dialog *pd) {
	pd->_copied = true;
	pd->_dialog->hide();
}

void Print_Options_Dialog::cancel_cb(Fl_Widget *, Print_Options_Dialog *pd) {
	pd->_canceled = true;
	pd->_dialog->hide();
}
