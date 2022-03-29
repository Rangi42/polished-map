#ifndef UTILS_H
#define UTILS_H

#include <cstdlib>
#include <cstdio>
#include <stdint.h>
#include <limits>
#include <cmath>
#include <string>
#include <string_view>
#include <algorithm>
#include <fstream>
#include <sstream>

#pragma warning(push, 0)
#include <FL/fl_draw.H>
#pragma warning(pop)

#ifdef _WIN32
#define DIR_SEP "\\"
#else
#define DIR_SEP "/"
#endif

#ifndef _countof
#define _countof(a) (sizeof(a) / sizeof(a[0]))
#endif

#define RANGE(x) std::begin(x), std::end(x)

typedef uint8_t size8_t;
typedef uint16_t size16_t;
typedef uint32_t size32_t;
typedef uint64_t size64_t;

extern const std::string whitespace;

bool equals_ignore_case(std::string_view s, std::string_view p);
bool starts_with(std::string_view s, std::string_view p);
bool ends_with(std::string_view s, std::string_view p);
bool ends_with_ignore_case(std::string_view s, std::string_view p);
bool ends_with_ignore_case(std::wstring_view s, std::wstring_view p);
void trim(std::string &s, const std::string &t = whitespace);
void lowercase(std::string &s);
bool leading_macro(std::istringstream &iss, std::string &macro, const char *v = NULL);
void remove_comment(std::string &s);
void remove_suffix(const char *n, char *s);
void before_suffix(const char *n, char *s);
void after_suffix(const char *n, char *s);
void remove_dot_ext(const char *f, char *s);
void add_dot_ext(const char *f, const char *ext, char *s);
int text_width(const char *l, int pad = 0);
bool file_exists(const char *f);
size_t file_size(const char *f);
size_t file_size(FILE *f);
int64_t file_modified(const char *f);
void open_ifstream(std::ifstream &ifs, const char *f);
void draw_outlined_text(const char *l, int x, int y, int w, int h, Fl_Align a, Fl_Color c, Fl_Color s);

#endif
