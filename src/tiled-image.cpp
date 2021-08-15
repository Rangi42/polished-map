#include <array>

#pragma warning(push, 0)
#include <FL/fl_types.h>
#include <FL/fl_utf8.h>
#include <FL/Fl_PNG_Image.H>
#pragma warning(pop)

#include "utils.h"
#include "tileset.h"
#include "tiled-image.h"

Tiled_Image::Tiled_Image(const char *f) : _tile_hues(NULL), _num_tiles(0), _result(Result::IMG_NULL) {
	if (!f) { return; }
	else if (ends_with_ignore_case(f, ".png")) { read_png_graphics(f); }
	else if (ends_with_ignore_case(f, ".2bpp")) { read_2bpp_graphics(f); }
	else if (ends_with_ignore_case(f, ".2bpp.lz")) { read_lz_graphics(f); }
}

Tiled_Image::~Tiled_Image() {
	delete [] _tile_hues;
}

Tiled_Image::Result Tiled_Image::read_png_graphics(const char *f) {
	Fl_PNG_Image png(f);
	if (png.fail()) { return (_result = Result::IMG_BAD_FILE); }

	int w = png.w(), h = png.h();
	if (w % TILE_SIZE || h % TILE_SIZE) { return (_result = Result::IMG_BAD_DIMS); }

	w /= TILE_SIZE;
	h /= TILE_SIZE;
	_num_tiles = w * h;
	if (_num_tiles > MAX_NUM_TILES) { return (_result = Result::IMG_TOO_LARGE); }

	png.desaturate();
	if (png.count() != 1) { return (_result = Result::IMG_NOT_GRAYSCALE); }

	delete [] _tile_hues;
	_tile_hues = new Hue[_num_tiles * TILE_AREA]();

	size_t hi = 0;
	int d = png.d();
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			for (int ty = 0; ty < TILE_SIZE; ty++) {
				for (int tx = 0; tx < TILE_SIZE; tx++) {
					long ti = ((y * TILE_SIZE + ty) * (w * TILE_SIZE) + (x * TILE_SIZE + tx)) * d;
					_tile_hues[hi++] = Color::mono_hue(png.array[ti]);
				}
			}
		}
	}

	return (_result = Result::IMG_OK);
}

Tiled_Image::Result Tiled_Image::read_2bpp_graphics(const char *f) {
	FILE *file = fl_fopen(f, "rb");
	if (!file) { return (_result = Result::IMG_BAD_FILE); }

	size_t n = file_size(file);
	if (n % BYTES_PER_2BPP_TILE) { fclose(file); return (_result = Result::IMG_BAD_DIMS); }

	uchar *data = new uchar[n];
	size_t r = fread(data, 1, n, file);
	fclose(file);
	if (r != n) { delete [] data; return (_result = Result::IMG_BAD_FILE); }

	return (_result = parse_2bpp_data(n, data));
}

// A rundown of Pokemon Crystal's LZ compression scheme:
enum class Lz_Command {
	// Control commands occupy bits 5-7.
	// Bits 0-4 serve as the first parameter n for each command.
	LZ_LITERAL,   // n values for n bytes
	LZ_ITERATE,   // one value for n bytes
	LZ_ALTERNATE, // alternate two values for n bytes
	LZ_BLANK,     // zero for n bytes
	// Repeater commands repeat any data that was just decompressed.
	// They take an additional signed parameter s to mark a relative starting point.
	// These wrap around (positive from the start, negative from the current position).
	LZ_REPEAT,    // n bytes starting from s
	LZ_FLIP,      // n bytes in reverse bit order starting from s
	LZ_REVERSE,   // n bytes backwards starting from s
	// The long command is used when 5 bits aren't enough. Bits 2-4 contain a new control code.
	// Bits 0-1 are appended to a new byte as 8-9, allowing a 10-bit parameter.
	LZ_LONG       // n is now 10 bits for a new control code
};

// If 0xff is encountered instead of a command, decompression ends.
#define LZ_END 0xff

static auto bit_flipped = ([]() constexpr {
	std::array<uchar, 256> a{};
	for (size_t i = 0; i < a.size(); i++) {
		for (size_t b = 0; b < 8; b++) {
			a[i] += ((i >> b) & 1) << (7 - b);
		}
	}
	return a;
})();

Tiled_Image::Result Tiled_Image::read_lz_graphics(const char *f) {
	FILE *file = fl_fopen(f, "rb");
	if (!file) { return (_result = Result::IMG_BAD_FILE); }

	size_t n = file_size(file);
	uchar *lz_data = new uchar[n];
	size_t r = fread(lz_data, 1, n, file);
	fclose(file);
	if (r != n) { delete [] lz_data; return (_result = Result::IMG_BAD_FILE); }

	uchar *twobpp_data = new uchar[MAX_NUM_TILES * BYTES_PER_2BPP_TILE]();
	size_t address = 0, marker = 0;
	uchar q[2];
	int offset;
	for (;;) {
		uchar b = lz_data[address++];
		if (b == LZ_END) { break; }
		if (marker >= MAX_NUM_TILES * BYTES_PER_2BPP_TILE) {
			delete [] lz_data;
			delete [] twobpp_data;
			return (_result = Result::IMG_TOO_LARGE);
		}
		Lz_Command cmd = (Lz_Command)((b & 0xe0) >> 5);
		int length = 0;
		if (cmd == Lz_Command::LZ_LONG) {
			cmd = (Lz_Command)((b & 0x1c) >> 2);
			length = (int)(b & 0x03) * 0x100;
			b = lz_data[address++];
			length += (int)b + 1;
		}
		else {
			length = (int)(b & 0x1f) + 1;
		}
		switch (cmd) {
		case Lz_Command::LZ_LITERAL:
			// Copy data directly.
			for (int i = 0; i < length; i++) {
				twobpp_data[marker++] = lz_data[address++];
			}
			break;
		case Lz_Command::LZ_ITERATE:
			// Write one byte repeatedly.
			b = lz_data[address++];
			for (int i = 0; i < length; i++) {
				twobpp_data[marker++] = b;
			}
			break;
		case Lz_Command::LZ_ALTERNATE:
			// Write alternating bytes.
			q[0] = lz_data[address++];
			q[1] = lz_data[address++];
			// Copy data directly.
			for (int i = 0; i < length; i++) {
				twobpp_data[marker++] = q[i & 1];
			}
			break;
		case Lz_Command::LZ_BLANK:
			// Write zeros.
			for (int i = 0; i < length; i++) {
				twobpp_data[marker++] = 0;
			}
			break;
		case Lz_Command::LZ_REPEAT:
			// Repeat bytes from output.
			b = lz_data[address++];
			offset = b >= 0x80 ? (int)marker - (int)(b & 0x7f) - 1 : (int)b * 0x100 + lz_data[address++];
			for (int i = 0; i < length; i++) {
				twobpp_data[marker++] = twobpp_data[offset + i];
			}
			break;
		case Lz_Command::LZ_FLIP:
			// Repeat flipped bytes from output.
			b = lz_data[address++];
			offset = b >= 0x80 ? (int)marker - (int)(b & 0x7f) - 1 : (int)b * 0x100 + lz_data[address++];
			for (int i = 0; i < length; i++) {
				b = twobpp_data[offset + i];
				twobpp_data[marker++] = bit_flipped[b];
			}
			break;
		case Lz_Command::LZ_REVERSE:
			// Repeat reversed bytes from output.
			b = lz_data[address++];
			offset = b >= 0x80 ? (int)marker - (int)(b & 0x7f) - 1 : (int)b * 0x100 + lz_data[address++];
			for (int i = 0; i < length; i++) {
				twobpp_data[marker++] = twobpp_data[offset - i];
			}
			break;
		case Lz_Command::LZ_LONG:
		default:
			delete [] lz_data;
			delete [] twobpp_data;
			return (_result = Result::IMG_BAD_CMD);
		}
	}

	delete [] lz_data;
	return (_result = parse_2bpp_data(marker, twobpp_data));
}

static void convert_2bytes_to_8hues(uchar b1, uchar b2, Hue *hues8) {
	// %ABCD_EFGH %abcd_efgh -> %Aa %Bb %Cc %Dd %Ee %Ff %GG %Hh
	for (int i = 0; i < 8; i++) {
		int j = 7 - i;
		hues8[i] = (Hue)((b1 >> j & 1) * 2 + (b2 >> j & 1));
	}
}

Tiled_Image::Result Tiled_Image::parse_2bpp_data(size_t n, uchar *data) {
	n /= BYTES_PER_2BPP_TILE;
	if (n > MAX_NUM_TILES) { delete [] data; return Result::IMG_TOO_LARGE; }

	_num_tiles = n;
	delete [] _tile_hues;
	_tile_hues = new Hue[_num_tiles * TILE_AREA]();

	for (size_t i = 0; i < _num_tiles; i++) {
		for (int j = 0; j < TILE_SIZE; j++) {
			uchar b1 = data[i * BYTES_PER_2BPP_TILE + j * 2];
			uchar b2 = data[i * BYTES_PER_2BPP_TILE + j * 2 + 1];
			convert_2bytes_to_8hues(b1, b2, _tile_hues + (i * TILE_SIZE + j) * 8);
		}
	}

	delete [] data;
	return Result::IMG_OK;
}
