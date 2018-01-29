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

static char *trim_suffix(const char *s) {
	// remove trailing ".t#", e.g. tileset "overworld.t2" -> name "overworld"
#ifdef _WIN32
	char *t = _strdup(s);
#else
	char *t = strdup(s);
#endif
	char *dot = strchr(t, '.');
	if (dot) { *dot = '\0'; }
	return t;
}

Fl_Preferences Config::global_config(Fl_Preferences::USER, PROGRAM_AUTHOR, PROGRAM_NAME);

bool Config::_monochrome = false, Config::_skip_tiles_60_to_7f = true;

int Config::get(const char *key, int default_) {
	int value;
	global_config.get(key, value, default_);
	return value;
}

void Config::set(const char *key, int value) {
	global_config.set(key, value);
}

const char *Config::gfx_tileset_dir() {
	return "gfx" DIR_SEP "tilesets" DIR_SEP;
}

const char *Config::palette_macro() {
	return "\ttilepal";
}

bool Config::project_path_from_blk_path(const char *blk_path, char *project_path) {
#ifdef _WIN32
	if (_splitpath_s(blk_path, NULL, 0, project_path, FL_PATH_MAX, NULL, 0, NULL, 0)) { return false; }
#else
	char *blk = strdup(blk_path);
	strcpy(project_path, dirname(blk));
	free(blk);
	strcat(project_path, DIR_SEP);
#endif
	int depth = -1;
	for (char *c = project_path; *c; c++) {
		depth += *c == '/' || *c == '\\';
	}
	char main_asm[FL_PATH_MAX] = {};
	for (int i = 0; i < depth; i++) {
		strcpy(main_asm, project_path);
		strcat(main_asm, "main.asm");
		if (file_exists(main_asm)) { // the project directory contains main.asm
			return true;
		}
		strcat(project_path, ".." DIR_SEP); // go up a level
	}
	return false;
}

void Config::palette_map_path(char *dest, const char *root, const char *tileset) {
	if (monochrome()) { return; }
	// try gfx/tilesets/*_palette_map.asm (POKECRYSTAL2018)
	sprintf(dest, "%s%s%s_palette_map.asm", root, gfx_tileset_dir(), tileset);
	if (file_exists(dest)) { return; }
	// try color/tilesets/*.asm (RPP)
	char *name = trim_suffix(tileset);
	sprintf(dest, "%scolor" DIR_SEP "tilesets" DIR_SEP "%s.asm", root, name);
	free(name);
	if (file_exists(dest)) { return; }
	// last resort: tilesets/*_palette_map.asm
	sprintf(dest, "%stilesets" DIR_SEP "%s_palette_map.asm", root, tileset);
}

void Config::tileset_path(char *dest, const char *root, const char *tileset) {
	// try gfx/tilesets/*.png
	sprintf(dest, "%s%s%s.png", root, gfx_tileset_dir(), tileset);
	if (file_exists(dest)) { return; }
	// try gfx/tilesets/*.2bpp
	sprintf(dest, "%s%s%s.2bpp", root, gfx_tileset_dir(), tileset);
	if (file_exists(dest)) { return; }
	// last resort: gfx/tilesets/*.2bpp.lz
	sprintf(dest, "%s%s%s.2bpp.lz", root, gfx_tileset_dir(), tileset);
}

void Config::tileset_png_path(char *dest, const char *root, const char *tileset) {
	sprintf(dest, "%s%s%s.png", root, gfx_tileset_dir(), tileset);
}

void Config::metatileset_path(char *dest, const char *root, const char *tileset) {
	// try data/tilesets/*_metatiles.bin (POKECRYSTAL2018)
	sprintf(dest, "%sdata" DIR_SEP "tilesets" DIR_SEP "%s_metatiles.bin", root, tileset);
	if (file_exists(dest)) { return; }
	// try gfx/blocksets/*.bst (RPP)
	char *name = trim_suffix(tileset);
	sprintf(dest, "%sgfx" DIR_SEP "blocksets" DIR_SEP "%s.bst", root, name);
	free(name);
	if (file_exists(dest)) { return; }
	// last resort: tilesets/*_metatiles.bin
	sprintf(dest, "%stilesets" DIR_SEP "%s_metatiles.bin", root, tileset);
}

void Config::map_constants_path(char *dest, const char *root) {
	// try constants/map_dimension_constants.asm
	sprintf(dest, "%sconstants" DIR_SEP "map_dimension_constants.asm", root);
	if (file_exists(dest)) { return; }
	// last resort: constants/map_constants.asm
	sprintf(dest, "%sconstants" DIR_SEP "map_constants.asm", root);
}

void Config::tileset_constants_path(char *dest, const char *root) {
	// try constants/tileset_constants.asm
	sprintf(dest, "%sconstants" DIR_SEP "tileset_constants.asm", root);
	if (file_exists(dest)) { return; }
	// try constants/tilemap_constants.asm
	sprintf(dest, "%sconstants" DIR_SEP "tilemap_constants.asm", root);
}
