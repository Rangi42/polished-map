#include <cstdio>
#include <cstring>

#include "utils.h"
#include "config.h"

const char *gfx_tileset_dir() {
	return "gfx\\tilesets\\";
}

void project_path_from_blk_path(char *path) {
	strcat(path, "..\\"); // go up from maps/
}

void palette_map_path(char *dest, const char *root, const char *tileset) {
	sprintf(dest, "%stilesets\\%s_palette_map.asm", root, tileset);
}

void tileset_path(char *dest, const char *root, const char *tileset) {
	// prefer png, then 2bpp, then 2bpp.lz
	sprintf(dest, "%s%s%s.png", root, gfx_tileset_dir(), tileset);
	if (file_exists(dest)) { return; }
	sprintf(dest, "%s%s%s.2bpp", root, gfx_tileset_dir(), tileset);
	if (file_exists(dest)) { return; }
	sprintf(dest, "%s%s%s.2bpp.lz", root, gfx_tileset_dir(), tileset);
}

void metatileset_path(char *dest, const char *root, const char *tileset) {
	sprintf(dest, "%stilesets\\%s_metatiles.bin", root, tileset);
}
