#include <cstdio>
#include <cstring>

#include "config.h"

void project_path_from_blk_path(char *path) {
	strcat(path, "..\\"); // go up from maps/
}

void palette_map_path(char *dest, const char *root, const char *tileset) {
	sprintf(dest, "%stilesets\\%s_palette_map.asm", root, tileset);
}

void tileset_path(char *dest, const char *root, const char *tileset) {
	sprintf(dest, /* "%sgfx\\tilesets\\%s.png" */ "%s..\\..\\Dropbox\\pkmn\\tilesets\\%s.png", root, tileset);
}

void metatileset_path(char *dest, const char *root, const char *tileset) {
	sprintf(dest, "%stilesets\\%s_metatiles.bin", root, tileset);
}
