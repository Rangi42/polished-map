#include <cstdio>
#include <cstring>

#pragma warning(push, 0)
#include <FL/filename.H>
#include <FL/Fl_Preferences.H>
#pragma warning(pop)

#include "version.h"
#include "utils.h"
#include "config.h"

Fl_Preferences global_config(Fl_Preferences::USER, PROGRAM_AUTHOR, PROGRAM_NAME);

const char *gfx_tileset_dir() {
	return "gfx\\tilesets\\";
}

bool project_path_from_blk_path(const char *blk_path, char *project_path) {
	if (_splitpath_s(blk_path, NULL, 0, project_path, FL_PATH_MAX, NULL, 0, NULL, 0)) { return false; }
	strcat(project_path, "..\\"); // go up from maps/
	return true;
}

void blk_path_from_project_path(const char *project_path, char *blk_path) {
	strcpy(blk_path, project_path);
	strcat(blk_path, "maps\\");
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


