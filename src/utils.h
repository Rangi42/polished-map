#ifndef UTILS_H
#define UTILS_H

#include <cstdlib>
#include <stdint.h>
#include <limits>
#include <cmath>
#include <string>

/* debug */
#include <iostream>
#include <sstream>
#include <windows.h>

#define DEBUGPRINT(X) do { \
	std::wstringstream wss_dbg_; \
	wss_dbg_ << X; \
	OutputDebugString(wss_dbg_.str().c_str()); \
} while (0)
/* end debug */

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef uint8_t size8_t;
typedef uint16_t size16_t;
typedef uint32_t size32_t;
typedef uint64_t size64_t;

extern const std::string whitespace;

bool starts_with(const std::string &s, const std::string &p);
bool ends_with(const std::string &s, const std::string &p);
void trim(std::string &s, const std::string &t = whitespace);
void remove_comment(std::string &s, char c = ';');
int text_width(const char *l, int pad);
int text_width(const char *l, int pad = 0);

#endif
