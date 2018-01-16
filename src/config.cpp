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
Config::Project Config::global_project(Config::Project::POKECRYSTAL2018);

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

const char *Config::map_macro() {
	return (global_project == POKERED || global_project == RPP) ? "mapconst" : "mapgroup";
}

const char *Config::palette_macro() {
	return "\ttilepal";
}

const char *Config::project_type() {
	switch (global_project) {
	case POKECRYSTAL:
		return "pokecrystal";
	case POKERED:
		return "pokered";
	case POLISHED:
		return "Polished Crystal";
	case RPP:
		return "Red++";
	case PRISM:
		return "Prism";
	case AXYLLIA:
		return "Axyllia";
	default:
		return "unsupported";
	}
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
	switch (global_project) {
	default:
	case POKECRYSTAL:
	case POKERED:
	case POLISHED:
	case RPP:
		strcat(project_path, ".." DIR_SEP); // go up from maps/
		return true;
	case PRISM:
		strcat(project_path, ".." DIR_SEP ".." DIR_SEP); // go up from maps/blk/
		return true;
	case AXYLLIA:
		const char *basedir = fl_filename_name(project_path);
		if (strcmp(basedir, "maps") == 0) {
			strcat(project_path, ".." DIR_SEP); // go up from maps/
		}
		else {
			strcat(project_path, ".." DIR_SEP ".." DIR_SEP); // go up from maps/<landmark>/
		}
		return true;
	}
}

void Config::blk_path_from_project_path(const char *project_path, char *blk_path) {
	strcpy(blk_path, project_path);
	switch (global_project) {
	default:
	case POKECRYSTAL:
	case POLISHED:
	case AXYLLIA:
		strcat(blk_path, "maps" DIR_SEP);
		return;
	case POKERED:
	case RPP:
		strcat(blk_path, "gfx" DIR_SEP "blocksets" DIR_SEP);
		return;
	case PRISM:
		strcat(blk_path, "maps" DIR_SEP "blk" DIR_SEP);
		return;
	}
}

void Config::palette_map_path(char *dest, const char *root, const char *tileset) {
	if (monochrome()) { return; }
	if (global_project == Project::RPP) {
		char *name = trim_suffix(tileset);
		sprintf(dest, "%scolor" DIR_SEP "tilesets" DIR_SEP "%s.asm", root, name);
		free(name);
	}
	else if (global_project == Project::POKECRYSTAL2018) {
		sprintf(dest, "%sgfx" DIR_SEP "tilesets" DIR_SEP "%s_palette_map.asm", root, tileset);
	}
	else {
		sprintf(dest, "%stilesets" DIR_SEP "%s_palette_map.asm", root, tileset);
	}
}

void Config::tileset_path(char *dest, const char *root, const char *tileset) {
	// prefer png, then 2bpp, then 2bpp.lz
	sprintf(dest, "%s%s%s.png", root, gfx_tileset_dir(), tileset);
	if (file_exists(dest)) { return; }
	sprintf(dest, "%s%s%s.2bpp", root, gfx_tileset_dir(), tileset);
	if (file_exists(dest)) { return; }
	sprintf(dest, "%s%s%s.2bpp.lz", root, gfx_tileset_dir(), tileset);
}

void Config::tileset_png_path(char *dest, const char *root, const char *tileset) {
	sprintf(dest, "%s%s%s.png", root, gfx_tileset_dir(), tileset);
}

void Config::metatileset_path(char *dest, const char *root, const char *tileset) {
	if (global_project == Project::POKERED || global_project == Project::RPP) {
		// remove trailing ".t#", e.g. tileset "overworld.t2" -> name "overworld"
		char *name = trim_suffix(tileset);
		sprintf(dest, "%sgfx" DIR_SEP "blocksets" DIR_SEP "%s.bst", root, name);
		free(name);
	}
	else if (global_project == Project::POKECRYSTAL2018) {
		sprintf(dest, "%sdata" DIR_SEP "tilesets" DIR_SEP "%s_metatiles.bin", root, tileset);
	}
	else {
		sprintf(dest, "%stilesets" DIR_SEP "%s_metatiles.bin", root, tileset);
	}
}

void Config::map_constants_path(char *dest, const char *root) {
	// prefer map_dimension_constants.asm to map_constants.asm
	sprintf(dest, "%sconstants" DIR_SEP "map_dimension_constants.asm", root);
	if (file_exists(dest)) { return; }
	sprintf(dest, "%sconstants" DIR_SEP "map_constants.asm", root);
}

void Config::tileset_constants_path(char *dest, const char *root) {
	// prefer tileset_constants.asm to tilemap_constants.asm
	sprintf(dest, "%sconstants" DIR_SEP "tileset_constants.asm", root);
	if (file_exists(dest)) { return; }
	sprintf(dest, "%sconstants" DIR_SEP "tilemap_constants.asm", root);
}

bool Config::monochrome() {
	return global_project == Project::POKERED;
}

bool Config::skip_tiles_60_to_7f() {
	return global_project == Project::POKECRYSTAL || global_project == Project::POKECRYSTAL2018 ||
		global_project == Project::PRISM || global_project == Project::AXYLLIA;
}

bool Config::nybble_palettes() {
	return global_project != Project::POLISHED && global_project != Project::RPP;
}
