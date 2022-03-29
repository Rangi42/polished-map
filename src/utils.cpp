#include <cstring>
#include <cctype>
#include <cwctype>
#include <algorithm>
#include <sys/stat.h>

#pragma warning(push, 0)
#include <FL/filename.H>
#include <FL/fl_utf8.h>
#pragma warning(pop)

#include "utils.h"

const std::string whitespace(" \f\n\r\t\v");

static bool cmp_ignore_case(const char &a, const char &b) {
	return tolower(a) == tolower(b);
}

bool equals_ignore_case(std::string_view s, std::string_view p) {
	return s.size() == p.size() && std::equal(RANGE(s), RANGE(p), cmp_ignore_case);
}

bool starts_with(std::string_view s, std::string_view p) {
	return !s.compare(0, p.size(), p);
}

bool ends_with(std::string_view s, std::string_view p) {
	return s.size() >= p.size() && !s.compare(s.size() - p.size(), p.size(), p);
}

bool ends_with_ignore_case(std::string_view s, std::string_view p) {
	if (s.size() < p.size()) { return false; }
	std::string_view ss = s.substr(s.size() - p.size());
	return std::equal(RANGE(ss), RANGE(p), cmp_ignore_case);
}

bool ends_with_ignore_case(std::wstring_view s, std::wstring_view p) {
	if (s.size() < p.size()) { return false; }
	std::wstring_view ss = s.substr(s.size() - p.size());
	return std::equal(RANGE(ss), RANGE(p), [](const wchar_t &a, const wchar_t &b) {
		return towlower(a) == towlower(b);
	});
}

void trim(std::string &s, const std::string &t) {
	std::string::size_type p = s.find_first_not_of(t);
	s.erase(0, p);
	p = s.find_last_not_of(t);
	s.erase(p + 1);
}

void lowercase(std::string &s) {
	std::transform(RANGE(s), s.begin(), [](char c) { return (char)tolower(c); });
}

bool leading_macro(std::istringstream &iss, std::string &macro, const char *v) {
	int first = iss.peek();
	bool indented = first == ' ' || first == '\t';
	if (indented) { iss >> std::ws >> macro >> std::ws; }
	return indented && (!v || macro == v);
}

void remove_comment(std::string &s) {
	size_t p = s.find(';');
	if (p != std::string::npos) {
		s.erase(p);
	}
}

void remove_suffix(const char *n, char *s) {
	strcpy(s, n);
	char *dot = strchr(s, '.');
	if (dot) { *dot = '\0'; }
}

void before_suffix(const char *n, char *s) {
	const char *dot = strchr(n, '.');
	strcpy(s, dot ? dot + 1 : "");
	char *comma = strchr(s, ',');
	if (comma) { *comma = '\0'; }
}

void after_suffix(const char *n, char *s) {
	const char *dot = strchr(n, '.');
	const char *comma = dot ? strchr(dot, ',') : NULL;
	strcpy(s, comma ? comma + 1 : "");
}

void remove_dot_ext(const char *f, char *s) {
	strcpy(s, fl_filename_name(f));
	char *dot = strchr(s, '.');
	if (dot) { *dot = '\0'; }
}

void add_dot_ext(const char *f, const char *ext, char *s) {
	strcpy(s, f);
	const char *e = fl_filename_ext(s);
	if (!e || !strlen(e)) {
		strcat(s, ext);
	}
}

int text_width(const char *l, int pad) {
	int lw = 0, lh = 0;
	fl_measure(l, lw, lh, 0);
	return lw + 2 * pad;
}

bool file_exists(const char *f) {
	return !fl_access(f, 4); // R_OK
}

size_t file_size(const char *f) {
	struct stat s;
	int r = fl_stat(f, &s);
	return r ? 0 : (size_t)s.st_size;
}

size_t file_size(FILE *f) {
#ifdef __CYGWIN__
#define stat64 stat
#define fstat64 fstat
#elif defined(_WIN32)
#define fileno _fileno
#define stat64 _stat32i64
#define fstat64 _fstat32i64
#endif
	struct stat64 s;
	int r = fstat64(fileno(f), &s);
	return r ? 0 : (size_t)s.st_size;
}

int64_t file_modified(const char *f) {
	if (!f) { return 0; }
	struct stat s;
	int r = fl_stat(f, &s);
	return r ? 0 : s.st_mtime;
}

void open_ifstream(std::ifstream &ifs, const char *f) {
#ifdef _WIN32
	wchar_t wf[FL_PATH_MAX] = {};
	fl_utf8towc(f, strlen(f), wf, sizeof(wf));
	ifs.open(wf);
#else
	ifs.open(f);
#endif
}

void draw_outlined_text(const char *l, int x, int y, int w, int h, Fl_Align a, Fl_Color c, Fl_Color s) {
	fl_color(s);
	fl_draw(l, x-1, y-1, w, h, a);
	fl_draw(l, x-1, y+1, w, h, a);
	fl_draw(l, x+1, y-1, w, h, a);
	fl_draw(l, x+1, y+1, w, h, a);
	fl_color(c);
	fl_draw(l, x, y, w, h, a);
}
