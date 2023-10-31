#ifndef METATILESET_H
#define METATILESET_H

#include "palette-map.h"
#include "tileset.h"
#include "map.h"
#include "metatile.h"

#define EMPTY_RGB 0xAB, 0xCD, 0xEF // intentionally leave out parentheses or brackets

#define MAX_NUM_METATILES 256

class Metatileset {
public:
	enum class Result { META_OK, META_NO_GFX, META_BAD_FILE, META_TOO_SHORT, META_TOO_LONG, META_NULL };
private:
	Tileset _tileset;
	Metatile *_metatiles[MAX_NUM_METATILES];
	size_t _num_metatiles;
	Result _result;
	bool _modified, _bin_collisions;
	int64_t _mod_time, _mod_time_coll;
public:
	Metatileset();
	~Metatileset();
	inline size_t size(void) const { return _num_metatiles; }
	void size(size_t n);
	inline Tileset &tileset(void) { return _tileset; }
	inline const Tileset &const_tileset(void) const { return _tileset; }
	inline Metatile *metatile(uint8_t id) { return _metatiles[id]; }
	inline Result result(void) const { return _result; }
	inline bool modified(void) const { return _modified; }
	inline void modified(bool m) { _modified = m; }
	inline bool other_modified(const char *f) const { return file_modified(f) > _mod_time; }
	inline bool other_modified_collisions(const char *f) const { return file_modified(f) > _mod_time_coll; }
	inline bool bin_collisions(void) const { return _bin_collisions; }
	inline void bin_collisions(bool b) { _bin_collisions = b; }
	bool uses_tile(uint8_t id) const;
	void trim_tileset(void);
	void clear(void);
	void draw_metatile(int x, int y, uint8_t id, bool zoom, bool show_priority) const;
	uchar *print_rgb(const Map &map) const;
	Result read_metatiles(const char *f);
	bool write_metatiles(const char *f);
	inline Result read_collisions(const char *f) { return _bin_collisions ? read_bin_collisions(f) : read_asm_collisions(f); }
	inline bool write_collisions(const char *f) { return _bin_collisions ? write_bin_collisions(f) : write_asm_collisions(f); }
	static const char *error_message(Result result);
private:
	Result read_asm_metatiles(const char *f);
	Result read_asm_collisions(const char *f);
	Result read_bin_collisions(const char *f);
	bool write_asm_collisions(const char *f);
	bool write_bin_collisions(const char *f);
};

#endif
