#ifndef DIRECTORY_CHOOSER_H
#define DIRECTORY_CHOOSER_H

#include "utils.h"

#ifdef _WIN32

#include <shobjidl.h>

class Directory_Chooser {
private:
	const char *_title;
	const char *_filename;
	IFileOpenDialog *_fod_ptr;
public:
	Directory_Chooser(int type);
	~Directory_Chooser();
	inline const char *title(void) const { return _title; }
	inline void title(const char *t) { _title = t; }
	inline const char *filename(void) const { return _filename; }
	int show();
};

#else

#pragma warning(push, 0)
#include <FL/Fl_Native_File_Chooser.H>
#pragma warning(pop)

typedef Fl_Native_File_Chooser Directory_Chooser;

#endif

#endif
