#include "config.h"
#include "tileset.h"
#include "image.h"

Tileset::Tileset() : _name(), _lighting(), _tiles(), _num_tiles(0), _num_roof_tiles(0), _result(GFX_NULL),
	_modified(false), _modified_roof(false) {
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
	_num_tiles = 0;
	for (size_t i = 0; i < MAX_NUM_TILES; i++) {
		_tiles[i]->clear();
	}
	clear_roof_graphics();
	_result = GFX_NULL;
	_modified = false;
	_modified_roof = false;
}

void Tileset::clear_roof_graphics() {
	_num_roof_tiles = 0;
	for (size_t i = 0; i < MAX_NUM_TILES; i++) {
		_roof_tiles[i]->clear();
	}
}

void Tileset::update_lighting(Lighting l) {
	_lighting = l;
	for (int i = 0; i < MAX_NUM_TILES; i++) {
		_tiles[i]->update_lighting(l);
		_roof_tiles[i]->update_lighting(l);
	}
}

uchar *Tileset::print_rgb(size_t w, size_t h, size_t n) const {
	uchar *buffer = new uchar[w * h * NUM_CHANNELS]();
	FILL(buffer, 0xff, w * h * NUM_CHANNELS);
	for (size_t i = 0; i < n; i++) {
		const Tile *t = _tiles[i];
		int ty = (i / TILES_PER_ROW) * TILE_SIZE, tx = (i % TILES_PER_ROW) * TILE_SIZE;
		print_tile_rgb(t, tx, ty, TILES_PER_ROW, buffer);
	}
	return buffer;
}

uchar *Tileset::print_roof_rgb(size_t w, size_t h) const {
	uchar *buffer = new uchar[w * h * NUM_CHANNELS]();
	FILL(buffer, 0xff, w * h * NUM_CHANNELS);
	for (size_t i = 0; i < NUM_ROOF_TILES; i++) {
		const Tile *t = _roof_tiles[i + FIRST_ROOF_TILE_ID];
		int ty = (i / ROOF_TILES_PER_ROW) * TILE_SIZE, tx = (i % ROOF_TILES_PER_ROW) * TILE_SIZE;
		print_tile_rgb(t, tx, ty, ROOF_TILES_PER_ROW, buffer);
	}
	return buffer;
}

void Tileset::read_tile(Tile *t, const Tiled_Image &ti, size_t i) {
	t->undefined(false);
	for (int ty = 0; ty < TILE_SIZE; ty++) {
		for (int tx = 0; tx < TILE_SIZE; tx++) {
			Hue h = ti.tile_hue(i, tx, ty);
			for (int pi = 0; pi < NUM_PALETTES; pi++) {
				Palette p = (Palette)pi;
				const uchar *rgb = Color::color(_lighting, p, h);
				t->pixel(p, tx, ty, h, rgb[0], rgb[1], rgb[2]);
			}
		}
	}
}

void Tileset::print_tile_rgb(const Tile *t, int tx, int ty, int n, uchar *buffer) const {
	for (int py = 0; py < TILE_SIZE; py++) {
		for (int px = 0; px < TILE_SIZE; px++) {
			Hue h = t->hue(px, py);
			const uchar *rgb = Color::monochrome_color(h);
			size_t j = ((ty + py) * n * TILE_SIZE + tx + px) * NUM_CHANNELS;
			buffer[j++] = rgb[0];
			buffer[j++] = rgb[1];
			buffer[j]   = rgb[2];
		}
	}
}

Tileset::Result Tileset::read_graphics(const char *f, Lighting l) {
	Tiled_Image ti(f);
	switch (ti.result()) {
	case Tiled_Image::IMG_OK: break;
	case Tiled_Image::IMG_NULL: return (_result = GFX_BAD_FILE);
	case Tiled_Image::IMG_BAD_FILE: return (_result = GFX_BAD_FILE);
	case Tiled_Image::IMG_BAD_EXT: return (_result = GFX_BAD_EXT);
	case Tiled_Image::IMG_BAD_DIMS: return (_result = GFX_BAD_DIMS);
	case Tiled_Image::IMG_TOO_SHORT: return (_result = GFX_TOO_SHORT);
	case Tiled_Image::IMG_TOO_LARGE: return (_result = GFX_TOO_LARGE);
	case Tiled_Image::IMG_NOT_GRAYSCALE: return (_result = GFX_NOT_GRAYSCALE);
	case Tiled_Image::IMG_BAD_CMD: return (_result = GFX_BAD_CMD);
	default: return (_result = GFX_BAD_FILE);
	}

	_num_tiles = ti.num_tiles();

	_lighting = l;
	for (size_t i = 0; i < _num_tiles; i++) {
		Tile *t = _tiles[i];
		read_tile(t, ti, i);
	}

	_modified = false;
	_modified_roof = false;

	return (_result = GFX_OK);
}

Tileset::Result Tileset::read_roof_graphics(const char *f) {
	Tiled_Image ti(f);
	switch (ti.result()) {
	case Tiled_Image::IMG_OK: break;
	case Tiled_Image::IMG_NULL: return GFX_BAD_FILE;
	case Tiled_Image::IMG_BAD_FILE: return GFX_BAD_FILE;
	case Tiled_Image::IMG_BAD_EXT: return GFX_BAD_EXT;
	case Tiled_Image::IMG_BAD_DIMS: return GFX_BAD_DIMS;
	case Tiled_Image::IMG_TOO_SHORT: return GFX_TOO_SHORT;
	case Tiled_Image::IMG_TOO_LARGE: return GFX_TOO_LARGE;
	case Tiled_Image::IMG_NOT_GRAYSCALE: return GFX_NOT_GRAYSCALE;
	case Tiled_Image::IMG_BAD_CMD: return GFX_BAD_CMD;
	default: return GFX_BAD_FILE;
	}

	if (ti.num_tiles() < NUM_ROOF_TILES) {
		return GFX_TOO_SHORT;
	}
	if (ti.num_tiles() > NUM_ROOF_TILES || FIRST_ROOF_TILE_ID + ti.num_tiles() > MAX_NUM_TILES) {
		return GFX_TOO_LARGE;
	}
	_num_roof_tiles = ti.num_tiles();

	for (size_t i = 0; i < _num_roof_tiles; i++) {
		int k = (int)i + FIRST_ROOF_TILE_ID;
		Tile *t = _roof_tiles[k];
		read_tile(t, ti, i);
	}

	return GFX_OK;
}

const char *Tileset::error_message(Result result) {
	switch (result) {
	case GFX_OK:
		return "OK.";
	case GFX_BAD_FILE:
		return "Cannot parse file format.";
	case GFX_BAD_EXT:
		return "Unknown file extension.";
	case GFX_BAD_DIMS:
		return "Image dimensions do not fit the tile grid.";
	case GFX_TOO_SHORT:
		return "Too few bytes.";
	case GFX_TOO_LARGE:
		return "Too many pixels.";
	case GFX_NOT_GRAYSCALE:
		return "Image cannot be made grayscale.";
	case GFX_BAD_CMD:
		return "Invalid LZ command.";
	case GFX_NULL:
		return "No graphics file chosen.";
	default:
		return "Unspecified error.";
	}
}

bool Tileset::write_graphics(const char *f) {
	Image::Result result = Image::write_tileset_image(f, *this);
	return !result;
}

bool Tileset::write_roof_graphics(const char *f) {
	Image::Result result = Image::write_roof_image(f, *this);
	return !result;
}
