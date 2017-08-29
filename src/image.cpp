#include <cstdio>
#include <string>
#include <sstream>
#include <png.h>
#include <zlib.h>

#pragma warning(push, 0)
#include <FL/Fl.H>
#include <FL/gl.h>
#include <FL/glu.h>
#include <FL/fl_draw.H>
#pragma warning(pop)

#include "utils.h"
#include "image.h"

const char *Image::FILE_CHOOSER_FILTER = "Portable Network Graphics File\t*.png\nBitmap File\t*.bmp\n"
	"Truevision TGA File\t*.tga\nPortable Pixmap File\t*.ppm\n";

Image::Image(const char *f, size_t w, size_t h, Fl_Widget *wgt) : _file(NULL), _buffer(NULL), _width(w), _height(h), _error(0) {
	// Images require a writable file
	_file = fl_fopen(f, "wb");
	if (_file == NULL) { return; }
	_buffer = new(std::nothrow) uchar[3 * w * h];
	if (_buffer == NULL) { return; }
	if (wgt) {
		fl_read_image(_buffer, wgt->x(), wgt->y(), (int)w, (int)h);
		// Flip buffer upside-down
		uchar *temp_row = new(std::nothrow) uchar[3 * w];
		if (temp_row == NULL) { return; }
		for (size_t i = 0; i < h / 2; i++) {
			uchar *top_row = &_buffer[3 * w * i];
			uchar *bottom_row = &_buffer[3 * w * (h - i - 1)];
			memcpy(temp_row, top_row, 3 * w);
			memcpy(top_row, bottom_row, 3 * w);
			memcpy(bottom_row, temp_row, 3 * w);
		}
		delete [] temp_row;
	}
	else {
		// Fill the pixel buffer from the OpenGL context, in row-major order from bottom to top
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadPixels(0, 0, (GLsizei)w, (GLsizei)h, GL_RGB, GL_UNSIGNED_BYTE, _buffer);
	}
}

Image::~Image() {
	if (_file) { fclose(_file); }
	delete _buffer;
}

size_t Image::write(Image::Format f) {
	if (error()) { return 0; }
	switch (f) {
	case PNG:
	default:
		return write_png();
	case BMP:
		return write_bmp();
	case TGA:
		return write_tga();
	case PPM:
		return write_ppm();
	}
}

size_t Image::write_png() {
	size_t n = 0;
	// Create the necessary PNG structures
	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png == NULL) {
		_error = 1;
		return n;
	}
	png_infop info = png_create_info_struct(png);
	if (info == NULL) {
		_error = 1;
		png_destroy_write_struct(&png, (png_infopp)NULL);
		return n;
	}
	png_init_io(png, _file);
	// Set compression options
	png_set_compression_level(png, Z_BEST_COMPRESSION);
	png_set_compression_mem_level(png, Z_BEST_COMPRESSION);
	png_set_compression_strategy(png, Z_DEFAULT_STRATEGY);
	png_set_compression_window_bits(png, 15);
	png_set_compression_method(png, Z_DEFLATED);
	png_set_compression_buffer_size(png, 8192);
	// Write the PNG IHDR chunk
	png_set_IHDR(png, info, (png_uint_32)_width, (png_uint_32)_height, 8, PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	// Write the other PNG header chunks
	png_write_info(png, info);
	// Write the RGB pixels in row-major order from top to bottom
	size_t row_size = 3 * _width;
	png_bytep png_row = new(std::nothrow) png_byte[row_size];
	if (png_row) {
		for (size_t i = _height; i-- > 0;) {
			size_t row = row_size * i;
			for (size_t j = 0; j < _width; j++) {
				size_t col = 3 * j;
				size_t px = row + col;
				// False positive "C6386: Write overrun" error with Visual Studio 2013 code analysis
				png_row[col] = _buffer[px];
				png_row[col+1] = _buffer[px+1];
				png_row[col+2] = _buffer[px+2];
				n += 3;
			}
			png_write_row(png, png_row);
		}
		png_write_end(png, NULL);
	}
	delete [] png_row;
	png_destroy_write_struct(&png, &info);
	png_free_data(png, info, PNG_FREE_ALL, -1);
	return n;
}

size_t Image::write_bmp() {
	size_t n = 0;
	size_t file_header_size = 14;
	size_t info_header_size = 40;
	size_t header_size = file_header_size + info_header_size;
	size_t row_size = 3 * _width;
	size_t row_pad = 4 - row_size % 4; // align rows to 32-bit boundaries
	if (row_pad == 4) { row_pad = 0; }
	size_t data_size = (row_size + row_pad) * _height;
	size_t data_pad = 4 - data_size % 4;
	if (data_pad == 4) { data_pad = 0; }
	size_t image_size = data_size + data_pad;
	size_t file_size = header_size + image_size;
	float x_dpi, y_dpi;
	Fl::screen_dpi(x_dpi, y_dpi);
	size32_t x_ppm = (size32_t)(x_dpi * INCHES_PER_METER);
	size32_t y_ppm = (size32_t)(y_dpi * INCHES_PER_METER);
	uchar file_header[14] = {
		'B', 'M',         // magic number
		LE32(file_size),  // file size
		LE16(0),          // reserved 1 (unused)
		LE16(0),          // reserved 2 (unused)
		LE32(header_size) // header size
	};
	uchar info_header[40] = {
		LE32(info_header_size), // info header size
		LE32(_width),           // image width
		LE32(_height),          // image height
		LE16(1),                // num color planes
		LE16(24),               // bits per pixel
		LE32(0),                // compression method (RGB)
		LE32(image_size),       // image size in bytes
		LE32(x_ppm),            // horizontal pixels per meter
		LE32(y_ppm),            // vertical pixels per meter
		LE32(0),                // num colors (ignored)
		LE32(0)                 // num important colors (ignored)
	};
	// Write the BMP file header
	n += fwrite(&file_header, sizeof(file_header), 1, _file);
	// Write the BMP info header
	n += fwrite(&info_header, sizeof(info_header), 1, _file);
	// Write the BGR pixels in row-major order from bottom to top
	for (size_t i = 0; i < _height; i++) {
		size_t row = row_size * i;
		for (size_t j = 0; j < _width; j++) {
			size_t px = row + 3 * j;
			fputc(_buffer[px+2], _file); // blue
			fputc(_buffer[px+1], _file); // green
			fputc(_buffer[px], _file);   // red
		}
		// Pad the rows to the nearest 4 bytes
		for (size_t j = 0; j < row_pad; j++) {
			fputc(0, _file);
		}
	}
	// Pad the pixel data to the nearest 4 bytes
	for (size_t i = 0; i < data_pad; i++) {
		fputc(0, _file);
	}
	n += image_size;
	return n;
}

size_t Image::write_tga() {
	size_t n = 0;
	uchar file_header[18] = {
		0,             // ID field size
		0,             // color map type (none)
		2,             // image type (RGB)
		LE16(0),       // color map offset
		LE16(0),       // color map size
		24,            // color map bits per pixel
		LE16(0),       // image x origin
		LE16(0),       // image y origin
		LE16(_width),  // image width
		LE16(_height), // image height
		24,            // bits per pixel
		0              // image descriptor
	};
	// Write the TGA header
	n += fwrite(&file_header, sizeof(file_header), 1, _file);
	// Write the BGR pixels in row-major order from bottom to top
	size_t row_size = 3 * _width;
	for (size_t i = 0; i < _height; i++) {
		size_t row = row_size * i;
		for (size_t j = 0; j < _width; j++) {
			size_t px = row + 3 * j;
			fputc(_buffer[px+2], _file); // blue
			fputc(_buffer[px+1], _file); // green
			fputc(_buffer[px], _file);   // red
		}
	}
	size_t data_size = row_size * _height;
	n += data_size;
	return n;
}

size_t Image::write_ppm() {
	size_t n = 0;
	// Write the PPM header
	std::ostringstream ss;
	ss << "P6 " << _width << " " << _height << " 255 ";
	std::string s = ss.str();
	size_t file_header_size = s.size();
	const char *file_header = s.c_str();
	n += fwrite(file_header, file_header_size, 1, _file);
	// Write the RGB pixels in row-major order from top to bottom
	size_t row_size = 3 * _width;
	for (size_t i = _height; i-- > 0;) {
		size_t row = row_size * i;
		n += fwrite(_buffer + row, row_size, 1, _file);
	}
	return n;
}
