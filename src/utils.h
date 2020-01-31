#ifndef UTILS_H
#define UTILS_H

#include <cstdlib>
#include <stdint.h>
#include <limits>
#include <cmath>
#include <string>

#if defined(__linux__) || defined(__unix__)
#define __LINUX__
#else
#undef __LINUX__
#endif

#ifdef _DEBUG

#pragma warning(push, 0)
#include <FL/fl_ask.H>
#pragma warning(pop)

#include <iostream>
#include <sstream>

#ifdef _WIN32

#include <windows.h>

#define DEBUGPRINT(X) \
	__pragma(warning(push)) \
	__pragma(warning(disable:4127)) \
	do { \
		std::wstringstream wss_dbg_; \
		wss_dbg_ << X << std::endl; \
		OutputDebugString(wss_dbg_.str().c_str()); \
	} while (0) \
	__pragma(warning(pop))

#endif

#else

#define DEBUGPRINT(X)

#endif

#ifdef _WIN32
#define DIR_SEP "\\"
#else
#define DIR_SEP "/"
#endif

#ifdef __APPLE__
#define COMMAND_KEY_PLUS "\xE2\x8C\x98" // UTF-8 encoding of U+2318 "PLACE OF INTEREST SIGN"
#define ALT_KEY_PLUS "\xE2\x8C\xA5" // UTF-8 encoding of U+2325 "OPTION KEY"
#define SHIFT_KEY_PLUS "\xE2\x87\xA7" // UTF-8 encoding of U+21E7 "UPWARDS WHITE ARROW"
#define COMMAND_SHIFT_KEYS_PLUS SHIFT_KEY_PLUS COMMAND_KEY_PLUS
#define COMMAND_ALT_KEYS_PLUS ALT_KEY_PLUS COMMAND_KEY_PLUS
#define ENTER_KEY_NAME "\xE2\x8C\xA4" // UTF-8 encoding of U+2324 "UP ARROWHEAD BETWEEN TWO HORIZONTAL BARS"
#else
#define COMMAND_KEY_PLUS "Ctrl+"
#define ALT_KEY_PLUS "Alt+"
#define SHIFT_KEY_PLUS "Shift+"
#define COMMAND_SHIFT_KEYS_PLUS COMMAND_KEY_PLUS SHIFT_KEY_PLUS
#define COMMAND_ALT_KEYS_PLUS COMMAND_KEY_PLUS ALT_KEY_PLUS
#define ENTER_KEY_NAME "Enter"
#endif

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#ifdef _WIN32
#define FILL(a, v, n) \
	__pragma(warning(push)) \
	__pragma(warning(disable:4127)) \
	do { \
		for (size_t __fill_i_ = 0; __fill_i_ < (size_t)(n); __fill_i_++) { \
			a[__fill_i_] = (v); \
		} \
	} while (0) \
	__pragma(warning(pop))
#else
#define FILL(a, v, n) \
	do { \
		for (size_t __fill_i_ = 0; __fill_i_ < (size_t)(n); __fill_i_++) { \
			a[__fill_i_] = (v); \
		} \
	} while (0)
#endif

#ifndef _countof
#define _countof(a) (sizeof(a) / sizeof(a[0]))
#endif

typedef uint8_t size8_t;
typedef uint16_t size16_t;
typedef uint32_t size32_t;
typedef uint64_t size64_t;

extern const std::string whitespace;

bool starts_with(const std::string &s, const std::string &p);
bool ends_with(const std::string &s, const std::string &p);
bool ends_with(const std::wstring &s, const std::wstring &p);
void trim(std::string &s, const std::string &t = whitespace);
void lowercase(std::string &s);
void remove_comment(std::string &s, char c = ';');
void remove_dot_ext(const char *f, char *s);
void add_dot_ext(const char *f, const char *ext, char *s);
int text_width(const char *l, int pad);
int text_width(const char *l, int pad = 0);
bool file_exists(const char *f);
size_t file_size(const char *f);

#endif
