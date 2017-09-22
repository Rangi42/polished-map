#ifndef METATILESET_H
#define METATILESET_H

#include "palette-map.h"
#include "tileset.h"
#include "map.h"
#include "metatile.h"

#define MAX_NUM_METATILES 256

class Metatileset {
public:
	enum Result { META_OK, META_NO_GFX, META_BAD_FILE, META_TOO_SHORT, META_NULL };
private:
	Tileset _tileset;
	Metatile *_metatiles[MAX_NUM_METATILES];
	size_t _num_metatiles;
	Result _result;
	bool _modified;
public:
	Metatileset();
	~Metatileset();
	size_t size(void) const { return _num_metatiles; }
	Tileset *tileset(void) { return &_tileset; }
	Metatile *metatile(uint8_t id) { return _metatiles[id]; }
	Result result(void) const { return _result; }
	inline bool modified(void) const { return _modified; }
	inline void modified(bool m) { _modified = m; }
	void clear(void);
	void draw_metatile(int x, int y, uint8_t id, bool z);
	uchar *print_rgb(const Map &map) const;
	Result read_metatiles(const char *f);
	static const char *error_message(Result result);
};

#endif
