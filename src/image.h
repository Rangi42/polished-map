#ifndef IMAGE_H
#define IMAGE_H

#define LE16(n) (uchar)((n) & 0xFF), (uchar)(((n)>>8) & 0xFF)
#define LE32(n) (uchar)((n) & 0xFF), (uchar)(((n)>>8) & 0xFF), (uchar)(((n)>>16) & 0xFF), (uchar)(((n)>>24) & 0xFF)

#define INCHES_PER_METER 39.3701

class Image {
public:
	enum Format { PNG, BMP, TGA, PPM };
	static const char *FILE_CHOOSER_FILTER;
private:
	FILE *_file;
	uchar *_buffer;
	size_t _width, _height;
	int _error;
public:
	Image(const char *f, size_t w, size_t h, Fl_Widget *wgt = NULL);
	~Image();
	size_t write(Format f);
	inline int error(void) { return ferror(_file) | _error; }
	inline void close(void) { fclose(_file); _file = NULL; }
private:
	Image(const Image &image); // Unimplemented copy constructor
	Image &operator=(const Image &image); // Unimplemented assignment operator
	size_t write_png(void);
	size_t write_bmp(void);
	size_t write_tga(void);
	size_t write_ppm(void);
};

#endif
