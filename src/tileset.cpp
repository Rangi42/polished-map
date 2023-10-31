#include "config.h"
#include "tileset.h"
#include "image.h"

Tileset::Tileset() : _name(), _palettes(), _palette_map(), _tiles(), _roof_tiles(), _num_tiles(0), _num_before_tiles(0),
	_num_mid_tiles(0), _num_roof_tiles(0), _result(Result::GFX_NULL), _modified(false), _modified_roof(false),
	_mod_time(0), _mod_time_before(0), _mod_time_after(0), _mod_time_roof(0) {
	for (size_t i = 0; i < MAX_NUM_TILES; i++) {
		_tiles[i] = new Tile((uint8_t)i);
		_roof_tiles[i] = new Tile((uint8_t)i);
	}
}

Tileset::~Tileset() {
	clear();
	for (size_t i = 0; i < MAX_NUM_TILES; i++) {
		delete _tiles[i];
		delete _roof_tiles[i];
	}
}

void Tileset::clear() {
	_name.clear();
	_palette_map.clear();
	_num_tiles = 0;
	_num_before_tiles = 0;
	_num_mid_tiles = 0;
	for (Tile *t : _tiles) {
		t->clear();
	}
	clear_roof_graphics();
	_result = Result::GFX_NULL;
	_modified = false;
	_modified_roof = false;
	_mod_time = _mod_time_before = _mod_time_after = _mod_time_roof = 0;
}

void Tileset::clear_roof_graphics() {
	_num_roof_tiles = 0;
	for (Tile *rt : _roof_tiles) {
		rt->clear();
	}
}

void Tileset::update_palettes(Palettes l) {
	_palettes = l;
	bool allow_256_tiles = Config::allow_256_tiles();
	for (int i = 0; i < MAX_NUM_TILES; i++) {
		int j = (!allow_256_tiles && i >= 0x60) ? (i >= 0xE0 ? i - 0x80 : i + 0x20) : i;
		_tiles[j]->update_palettes(l);
		_roof_tiles[j]->update_palettes(l);
	}
}

uchar *Tileset::print_rgb(size_t w, size_t h, size_t off, size_t n) const {
	uchar *buffer = new uchar[w * h * NUM_CHANNELS]();
	std::fill_n(buffer, w * h * NUM_CHANNELS, (uchar)0xff);
	bool allow_256_tiles = Config::allow_256_tiles();
	for (size_t i = 0; i < n; i++) {
		size_t j = i + off;
		if (!allow_256_tiles && j == 0x60) { i += 0x1f; continue; }
		const Tile *t = _tiles[j];
		int ty = (i / TILES_PER_ROW) * TILE_SIZE, tx = (i % TILES_PER_ROW) * TILE_SIZE;
		if (!allow_256_tiles && j >= 0x80) { ty -= 2 * TILE_SIZE; }
		print_tile_rgb(t, tx, ty, TILES_PER_ROW, buffer);
	}
	return buffer;
}

uchar *Tileset::print_roof_rgb(size_t w, size_t h) const {
	uchar *buffer = new uchar[w * h * NUM_CHANNELS]();
	std::fill_n(buffer, w * h * NUM_CHANNELS, (uchar)0xff);
	for (size_t i = 0; i < NUM_ROOF_TILES; i++) {
		const Tile *t = _roof_tiles[i + FIRST_ROOF_TILE_ID];
		int ty = (i / ROOF_TILES_PER_ROW) * TILE_SIZE, tx = (i % ROOF_TILES_PER_ROW) * TILE_SIZE;
		print_tile_rgb(t, tx, ty, ROOF_TILES_PER_ROW, buffer);
	}
	return buffer;
}

void Tileset::read_tile(Tile *t, const Tiled_Image &ti, uint8_t i, size_t j) {
	Palette p = _palette_map.palette(i);
	t->palette(p);
	for (int ty = 0; ty < TILE_SIZE; ty++) {
		for (int tx = 0; tx < TILE_SIZE; tx++) {
			Hue h = ti.tile_hue(j, tx, ty);
			const uchar *rgb = Color::color(_palettes, p, h);
			t->pixel(tx, ty, h, rgb[0], rgb[1], rgb[2]);
		}
	}
}

void Tileset::print_tile_rgb(const Tile *t, int tx, int ty, int n, uchar *buffer) {
	for (int py = 0; py < TILE_SIZE; py++) {
		for (int px = 0; px < TILE_SIZE; px++) {
			Hue h = t->hue(px, py);
			uchar c;
			switch (h) {
			case Hue::BLACK: c = 0x00; break;
			case Hue::DARK:  c = 0x55; break;
			case Hue::LIGHT: c = 0xAA; break;
			case Hue::WHITE: default: c = 0xFF;
			}
			size_t j = ((ty + py) * n * TILE_SIZE + tx + px) * NUM_CHANNELS;
			buffer[j++] = c;
			buffer[j++] = c;
			buffer[j] = c;
		}
	}
}

Tileset::Result Tileset::convert_tiled_image_result(Tiled_Image::Result r) {
	switch (r) {
	case Tiled_Image::Result::IMG_OK: return Result::GFX_OK;
	case Tiled_Image::Result::IMG_NULL: return (_result = Result::GFX_BAD_FILE);
	case Tiled_Image::Result::IMG_BAD_FILE: return (_result = Result::GFX_BAD_FILE);
	case Tiled_Image::Result::IMG_BAD_EXT: return (_result = Result::GFX_BAD_EXT);
	case Tiled_Image::Result::IMG_BAD_DIMS: return (_result = Result::GFX_BAD_DIMS);
	case Tiled_Image::Result::IMG_TOO_SHORT: return (_result = Result::GFX_TOO_SHORT);
	case Tiled_Image::Result::IMG_TOO_LARGE: return (_result = Result::GFX_TOO_LARGE);
	case Tiled_Image::Result::IMG_NOT_GRAYSCALE: return (_result = Result::GFX_NOT_GRAYSCALE);
	case Tiled_Image::Result::IMG_BAD_CMD: return (_result = Result::GFX_BAD_CMD);
	default: return (_result = Result::GFX_BAD_FILE);
	}
}

Tileset::Result Tileset::read_graphics(const char *f, const char *bf, const char *af, Palettes l) {
	Tiled_Image bti(bf);
	size_t bn = bti.num_tiles();
	if (bf && convert_tiled_image_result(bti.result()) != Result::GFX_OK) { return _result; }

	Tiled_Image ti(f);
	size_t mn = ti.num_tiles();
	if (convert_tiled_image_result(ti.result()) != Result::GFX_OK) { return _result; }

	Tiled_Image ati(af);
	size_t an = ati.num_tiles();
	if (af && convert_tiled_image_result(ati.result()) != Result::GFX_OK) { return _result; }

	_num_before_tiles = ((bn + TILES_PER_ROW - 1) / TILES_PER_ROW) * TILES_PER_ROW;
	_num_mid_tiles = an ? ((mn + TILES_PER_ROW - 1) / TILES_PER_ROW) * TILES_PER_ROW : mn;
	_num_tiles = _num_before_tiles + (an ? _num_mid_tiles + an : mn);

	_palettes = l;
	bool allow_256_tiles = Config::allow_256_tiles();
	for (int i = 0; i < MAX_NUM_TILES; i++) {
		int j = (!allow_256_tiles && i >= 0x60) ? (i >= 0xE0 ? i - 0x80 : i + 0x20) : i;
		Tile *t = _tiles[j];
		if ((size_t)i < bn) {
			read_tile(t, bti, (uint8_t)i, (size_t)i);
		}
		else if ((size_t)i >= _num_before_tiles && (size_t)i < _num_before_tiles + mn) {
			read_tile(t, ti, (uint8_t)i, (size_t)i - _num_before_tiles);
		}
		else if ((size_t)i >= _num_before_tiles + _num_mid_tiles) {
			read_tile(t, ati, (uint8_t)i, (size_t)i - _num_before_tiles - _num_mid_tiles);
		}
	}

	_modified = false;
	_modified_roof = false;
	_mod_time = file_modified(f);
	_mod_time_before = file_modified(bf);
	_mod_time_after = file_modified(af);

	return (_result = Result::GFX_OK);
}

Tileset::Result Tileset::read_roof_graphics(const char *f) {
	Tiled_Image ti(f);
	switch (ti.result()) {
	case Tiled_Image::Result::IMG_OK: break;
	case Tiled_Image::Result::IMG_NULL: return Result::GFX_BAD_FILE;
	case Tiled_Image::Result::IMG_BAD_FILE: return Result::GFX_BAD_FILE;
	case Tiled_Image::Result::IMG_BAD_EXT: return Result::GFX_BAD_EXT;
	case Tiled_Image::Result::IMG_BAD_DIMS: return Result::GFX_BAD_DIMS;
	case Tiled_Image::Result::IMG_TOO_SHORT: return Result::GFX_TOO_SHORT;
	case Tiled_Image::Result::IMG_TOO_LARGE: return Result::GFX_TOO_LARGE;
	case Tiled_Image::Result::IMG_NOT_GRAYSCALE: return Result::GFX_NOT_GRAYSCALE;
	case Tiled_Image::Result::IMG_BAD_CMD: return Result::GFX_BAD_CMD;
	default: return Result::GFX_BAD_FILE;
	}

	if (ti.num_tiles() < NUM_ROOF_TILES) {
		return Result::GFX_TOO_SHORT;
	}
	if (ti.num_tiles() > NUM_ROOF_TILES || FIRST_ROOF_TILE_ID + ti.num_tiles() > MAX_NUM_TILES) {
		return Result::GFX_TOO_LARGE;
	}
	_num_roof_tiles = ti.num_tiles();

	bool allow_256_tiles = Config::allow_256_tiles();
	for (size_t i = 0; i < _num_roof_tiles; i++) {
		int k = (int)i + FIRST_ROOF_TILE_ID;
		int j = (!allow_256_tiles && k >= 0x60) ? (k >= 0xE0 ? k - 0x80 : k + 0x20) : k;
		Tile *t = _roof_tiles[j];
		read_tile(t, ti, (uint8_t)k, i);
	}

	_mod_time_roof = file_modified(f);

	return Result::GFX_OK;
}

const char *Tileset::error_message(Result result) {
	switch (result) {
	case Result::GFX_OK:
		return "OK.";
	case Result::GFX_BAD_FILE:
		return "Cannot parse file format.";
	case Result::GFX_BAD_EXT:
		return "Unknown file extension.";
	case Result::GFX_BAD_DIMS:
		return "Image dimensions do not fit the tile grid.";
	case Result::GFX_TOO_SHORT:
		return "Too few bytes.";
	case Result::GFX_TOO_LARGE:
		return "Too many pixels.";
	case Result::GFX_NOT_GRAYSCALE:
		return "Image cannot be made grayscale.";
	case Result::GFX_BAD_CMD:
		return "Invalid LZ command.";
	case Result::GFX_NULL:
		return "No graphics file chosen.";
	default:
		return "Unspecified error.";
	}
}

const char *Tileset::write_graphics(const char *f, const char *bf, const char *af) {
	if (bf[0]) {
		Image::Result b_result = Image::write_tileset_image(bf, *this, 0, _num_before_tiles);
		if (b_result != Image::Result::IMAGE_OK) { return bf; }
		_mod_time_before = file_modified(bf);
	}
	if (af[0]) {
		Image::Result result = Image::write_tileset_image(f, *this, _num_before_tiles, _num_mid_tiles);
		if (result != Image::Result::IMAGE_OK) { return f; }
		_mod_time = file_modified(f);
		Image::Result a_result = Image::write_tileset_image(af, *this, _num_before_tiles + _num_mid_tiles, 0);
		if (a_result != Image::Result::IMAGE_OK) { return af; }
		_mod_time_after = file_modified(af);
		return NULL;
	}
	else {
		Image::Result result = Image::write_tileset_image(f, *this, _num_before_tiles, 0);
		if (result != Image::Result::IMAGE_OK) { return f; }
		_mod_time = file_modified(f);
		return NULL;
	}
}

bool Tileset::write_roof_graphics(const char *f) {
	Image::Result result = Image::write_roof_image(f, *this);
	if (result != Image::Result::IMAGE_OK) { return false; }
	_mod_time_roof = file_modified(f);
	return true;
}
