#include <cstring>
#include <algorithm>
#include <sys/stat.h>

#pragma warning(push, 0)
#include <FL/fl_draw.H>
#include <FL/filename.H>
#include <FL/fl_utf8.h>
#pragma warning(pop)

#include "utils.h"

const std::string whitespace(" \f\n\r\t\v");

bool starts_with(std::string_view s, std::string_view p) {
	return !s.compare(0, p.size(), p);
}

bool ends_with(std::string_view s, std::string_view p) {
	return s.size() >= p.size() && !s.compare(s.size() - p.size(), p.size(), p);
}

bool ends_with(std::wstring_view s, std::wstring_view p) {
	return s.size() >= p.size() && !s.compare(s.size() - p.size(), p.size(), p);
}

void trim(std::string &s, const std::string &t) {
	std::string::size_type p = s.find_first_not_of(t);
	s.erase(0, p);
	p = s.find_last_not_of(t);
	s.erase(p + 1);
}

void lowercase(std::string& s) {
	std::transform(s.begin(), s.end(), s.begin(), [](char c) { return (char)std::tolower(c); });
}

void remove_comment(std::string &s, char c) {
	size_t p = s.find(c);
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
#ifdef __CYGWIN__
#define stat64 stat
#elif defined(_WIN32)
#define stat64 _stat32i64
#endif
	struct stat64 s;
	int r = stat64(f, &s);
	return r ? 0 : (size_t)s.st_size;
}

void open_ifstream(std::ifstream &ifs, const char *f) {
#ifdef _WIN32
	wchar_t *wf = utf8towchar(f);
	ifs.open(wf);
	free(wf);
#else
	ifs.open(f);
#endif
}

#ifdef _WIN32

// utf8towchar and wchartoutf8 copied from Fl_Native_File_Chooser_WIN32.cxx

wchar_t *utf8towchar(const char *in) {
	if (in == NULL) { return NULL; }
	wchar_t *wout = NULL;
	int lwout = 0;
	int wlen = MultiByteToWideChar(CP_UTF8, 0, in, -1, NULL, 0);
	if (wlen > lwout) {
		lwout = wlen;
		wout = (wchar_t *)malloc(lwout * sizeof(wchar_t));
	}
	MultiByteToWideChar(CP_UTF8, 0, in, -1, wout, wlen);
	return wout;
}

char *wchartoutf8(const wchar_t *in) {
	if (in == NULL) { return NULL; }
	char *out = NULL;
	int lchar = 0;
	int utf8len  = WideCharToMultiByte(CP_UTF8, 0, in, -1, NULL, 0, NULL, NULL);
	if (utf8len > lchar) {
		lchar = utf8len;
		out = (char *)malloc(lchar);
	}
	WideCharToMultiByte(CP_UTF8, 0, in, -1, out, utf8len, NULL, NULL);
	return out;
}

#endif
