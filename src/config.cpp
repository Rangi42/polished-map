#include <cstdio>
#include <cstring>

#pragma warning(push, 0)
#include <FL/filename.H>
#include <FL/Fl_Preferences.H>
#pragma warning(pop)

#include "version.h"
#include "utils.h"
#include "config.h"

#ifndef _WIN32
#include <libgen.h>
#endif

Fl_Preferences global_config(Fl_Preferences::USER, PROGRAM_AUTHOR, PROGRAM_NAME);

const char *gfx_tileset_dir() {
	return "gfx" DIR_SEP "tilesets" DIR_SEP;
}

bool project_path_from_blk_path(const char *blk_path, char *project_path, bool prism) {
#ifdef _WIN32
	if (_splitpath_s(blk_path, NULL, 0, project_path, FL_PATH_MAX, NULL, 0, NULL, 0)) { return false; }
#else
	char *blk = strdup(blk_path);
	strcpy(project_path, dirname(blk));
	free(blk);
	strcat(project_path, DIR_SEP);
#endif
	strcat(project_path, prism ? ".." DIR_SEP ".." DIR_SEP : ".." DIR_SEP); // go up from maps/
	return true;
}

void blk_path_from_project_path(const char *project_path, char *blk_path, bool prism) {
	strcpy(blk_path, project_path);
	strcat(blk_path, prism ? "maps" DIR_SEP "blk" DIR_SEP : "maps" DIR_SEP);
}

void palette_map_path(char *dest, const char *root, const char *tileset) {
	sprintf(dest, "%stilesets" DIR_SEP "%s_palette_map.asm", root, tileset);
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
	sprintf(dest, "%stilesets" DIR_SEP "%s_metatiles.bin", root, tileset);
}

void map_constants_path(char *dest, const char *root) {
	// prefer map_dimension_constants.asm to map_constants.asm
	sprintf(dest, "%sconstants" DIR_SEP "map_dimension_constants.asm", root);
	if (file_exists(dest)) { return; }
	sprintf(dest, "%sconstants" DIR_SEP "map_constants.asm", root);
}
