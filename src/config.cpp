#include <cstdio>
#include <cstring>

#pragma warning(push, 0)
#include <FL/filename.H>
#pragma warning(pop)

#include "utils.h"
#include "config.h"

bool Config::_512_tiles = false, Config::_0_before_1 = false, Config::_drag_and_drop = true;
bool Config::_print_grid = false, Config::_print_ids = false, Config::_print_priority = false, Config::_print_events = false;

// wOverworldMapBlocks (aka OverworldMap or wOverworldMap) buffer size in WRAM
size_t Config::_overworld_map_size = 1300;

const char *Config::gfx_tileset_dir() {
	return "gfx" DIR_SEP "tilesets" DIR_SEP;
}

const char *Config::gfx_roof_dir() {
	return "gfx" DIR_SEP "tilesets" DIR_SEP "roofs" DIR_SEP;
}

const char *Config::palette_macro() {
	return "\ttilepal";
}

bool Config::project_path_from_blk_path(const char *blk_path, char *project_path) {
	char scratch_path[FL_PATH_MAX] = {};
	fl_filename_absolute(scratch_path, blk_path);
	char main_asm[FL_PATH_MAX] = {};
	for (;;) {
		char *pivot = strrchr(scratch_path, *DIR_SEP);
		if (!pivot) { return false; }
		*pivot = '\0';
		// Make sure there's enough room for "/main.asm\0" or "/layout.link\0"
		if (pivot - scratch_path + 13 > FL_PATH_MAX) { return false; }
		strcpy(main_asm, scratch_path);
		strcat(main_asm, DIR_SEP "main.asm");
		if (!file_exists(main_asm)) {
			strcpy(main_asm, scratch_path);
			strcat(main_asm, DIR_SEP "layout.link");
		}
		if (file_exists(main_asm)) { // the project directory contains main.asm or layout.link
			strcat(scratch_path, DIR_SEP);
			strcpy(project_path, scratch_path);
			return true;
		}
	}
}

static void _attributes_path(char *dest, const char *root, const char *tileset) {
	// try data/tilesets/*_attributes.bin (pokecrystal)
	sprintf(dest, "%sdata" DIR_SEP "tilesets" DIR_SEP "%s_attributes.bin", root, tileset);
	if (file_exists(dest)) { return; }
	// last resort: tilesets/*_attributes.bin (old pokecrystal)
	sprintf(dest, "%stilesets" DIR_SEP "%s_attributes.bin", root, tileset);
}

void Config::attributes_path(char *dest, const char *root, const char *tileset) {
	// try paths with full tileset name
	_attributes_path(dest, root, tileset);
	if (file_exists(dest)) { return; }
	// retry paths with trimmed suffix
	char name[FL_PATH_MAX] = {};
	remove_suffix(tileset, name);
	_attributes_path(dest, root, name);
}

void Config::tileset_path(char *dest, const char *root, const char *tileset) {
	// try gfx/tilesets/*.png (pokecrystal)
	sprintf(dest, "%s%s%s.png", root, gfx_tileset_dir(), tileset);
	if (file_exists(dest)) { return; }
	// try gfx/tilesets/*.2bpp
	sprintf(dest, "%s%s%s.2bpp", root, gfx_tileset_dir(), tileset);
	if (file_exists(dest)) { return; }
	// last resort: gfx/tilesets/*.2bpp.lz
	sprintf(dest, "%s%s%s.2bpp.lz", root, gfx_tileset_dir(), tileset);
}

bool Config::tileset_before_path(char *dest, const char *root, const char *tileset) {
	char before[FL_PATH_MAX] = {};
	before_suffix(tileset, before);
	if (!before[0]) { return false; }
	tileset_path(dest, root, before);
	return file_exists(dest);
}

bool Config::tileset_after_path(char *dest, const char *root, const char *tileset) {
	char after[FL_PATH_MAX] = {};
	after_suffix(tileset, after);
	if (!after[0]) { return false; }
	tileset_path(dest, root, after);
	return file_exists(dest);
}

void Config::tileset_png_paths(char *dest, char *b_dest, char *a_dest, const char *root, const char *tileset) {
	sprintf(dest, "%s%s%s.png", root, gfx_tileset_dir(), tileset);
	char before[FL_PATH_MAX] = {}, after[FL_PATH_MAX] = {};
	before_suffix(tileset, before);
	after_suffix(tileset, after);
	if (before[0]) {
		sprintf(b_dest, "%s%s%s.png", root, gfx_tileset_dir(), before);
	}
	else {
		b_dest[0] = '\0';
	}
	if (after[0]) {
		sprintf(a_dest, "%s%s%s.png", root, gfx_tileset_dir(), after);
	}
	else {
		a_dest[0] = '\0';
	}
}

void Config::roof_path(char *dest, const char *root, const char *roof) {
	// try gfx/tilesets/roofs/*.png
	sprintf(dest, "%s%s%s.png", root, gfx_roof_dir(), roof);
	if (file_exists(dest)) { return; }
	// try gfx/tilesets/roofs/*.2bpp
	sprintf(dest, "%s%s%s.2bpp", root, gfx_roof_dir(), roof);
	if (file_exists(dest)) { return; }
	// last resort: gfx/tilesets/roofs/*.2bpp.lz
	sprintf(dest, "%s%s%s.2bpp.lz", root, gfx_roof_dir(), roof);
}

void Config::roof_png_path(char *dest, const char *root, const char *roof) {
	sprintf(dest, "%s%s%s.png", root, gfx_roof_dir(), roof);
}

static void _metatileset_path(char *dest, const char *root, const char *tileset) {
	// try data/tilesets/*_metatiles.bin (pokecrystal)
	sprintf(dest, "%sdata" DIR_SEP "tilesets" DIR_SEP "%s_metatiles.bin", root, tileset);
	if (file_exists(dest)) { return; }
	// last resort: tilesets/*_metatiles.bin (old pokecrystal)
	sprintf(dest, "%stilesets" DIR_SEP "%s_metatiles.bin", root, tileset);
}

void Config::metatileset_path(char *dest, const char *root, const char *tileset) {
	// try paths with full tileset name
	_metatileset_path(dest, root, tileset);
	if (file_exists(dest)) { return; }
	// retry paths with trimmed suffix
	char name[FL_PATH_MAX] = {};
	remove_suffix(tileset, name);
	_metatileset_path(dest, root, name);
}

static bool _collisions_path(char *dest, const char *root, const char *tileset) {
	// try data/tilesets/*_collision.asm (pokecrystal)
	sprintf(dest, "%sdata" DIR_SEP "tilesets" DIR_SEP "%s_collision.asm", root, tileset);
	if (file_exists(dest)) { return false; }
	// try data/tilesets/*_collision.inc (SECTION-split pokecrystal)
	sprintf(dest, "%sdata" DIR_SEP "tilesets" DIR_SEP "%s_collision.inc", root, tileset);
	if (file_exists(dest)) { return false; }
	// try tilesets/*_collision.asm (old pokecrystal, converted from .bin)
	sprintf(dest, "%stilesets" DIR_SEP "%s_collision.asm", root, tileset);
	if (file_exists(dest)) { return false; }
	// last resort: tilesets/*_collision.bin (old pokecrystal)
	sprintf(dest, "%stilesets" DIR_SEP "%s_collision.bin", root, tileset);
	return true;
}

bool Config::collisions_path(char *dest, const char *root, const char *tileset) {
	// try paths with full tileset name
	bool bin_collisions = _collisions_path(dest, root, tileset);
	if (file_exists(dest)) { return bin_collisions; }
	// retry paths with trimmed suffix
	char name[FL_PATH_MAX] = {};
	remove_suffix(tileset, name);
	return _collisions_path(dest, root, name);
}

void Config::map_constants_path(char *dest, const char *root) {
	// try constants/map_dimension_constants.asm (Prism)
	sprintf(dest, "%sconstants" DIR_SEP "map_dimension_constants.asm", root);
	if (file_exists(dest)) { return; }
	// try constants/map_constants.asm (pokecrystal)
	sprintf(dest, "%sconstants" DIR_SEP "map_constants.asm", root);
	if (file_exists(dest)) { return; }
	// last resort: constants/map_constants.inc (SECTION-split pokecrystal)
	sprintf(dest, "%sconstants" DIR_SEP "map_constants.inc", root);
}

void Config::map_headers_path(char *dest, const char *root) {
	// try data/maps/maps.asm (pokecrystal)
	sprintf(dest, "%sdata" DIR_SEP "maps" DIR_SEP "maps.asm", root);
	if (file_exists(dest)) { return; }
	// last resort: maps/map_headers.asm (old pokecrystal)
	sprintf(dest, "%smaps" DIR_SEP "map_headers.asm", root);
}

void Config::event_script_path(char *dest, const char *root, const char *map_name) {
	sprintf(dest, "%smaps" DIR_SEP "%s.asm", root, map_name);
}

void Config::tileset_constants_path(char *dest, const char *root) {
	// try constants/tileset_constants.asm (pokecrystal)
	sprintf(dest, "%sconstants" DIR_SEP "tileset_constants.asm", root);
	if (file_exists(dest)) { return; }
	// try constants/tileset_constants.inc (SECTION-split pokecrystal)
	sprintf(dest, "%sconstants" DIR_SEP "tileset_constants.inc", root);
	if (file_exists(dest)) { return; }
	// last resort: constants/tilemap_constants.asm (old pokecrystal)
	sprintf(dest, "%sconstants" DIR_SEP "tilemap_constants.asm", root);
}

void Config::bg_tiles_pal_path(char *dest, const char *root) {
	// try gfx/tilesets/bg_tiles.pal (pokecrystal)
	sprintf(dest, "%sgfx" DIR_SEP "tilesets" DIR_SEP "bg_tiles.pal", root);
	if (file_exists(dest)) { return; }
	// last resort: tilesets/bg.pal (old pokecrystal)
	sprintf(dest, "%stilesets" DIR_SEP "bg.pal", root);
}

void Config::roofs_pal_path(char *dest, const char *root) {
	// try gfx/tilesets/roofs.pal (pokecrystal)
	sprintf(dest, "%sgfx" DIR_SEP "tilesets" DIR_SEP "roofs.pal", root);
	if (file_exists(dest)) { return; }
	// last resort: tilesets/roof.pal (old pokecrystal)
	sprintf(dest, "%stilesets" DIR_SEP "roof.pal", root);
}

void Config::special_pal_path(char *dest, const char *root, const char *filename, const char *landmark, const char *tileset) {
	// try unique map palette
	if (filename) {
		strcpy(dest, filename);
		fl_filename_setext(dest, FL_PATH_MAX, ".pal");
		if (file_exists(dest)) { return; }
	}
	// try unique landmark palette
	if (landmark) {
		sprintf(dest, "%s%s%s.pal", root, gfx_tileset_dir(), landmark);
		if (file_exists(dest)) { return; }
	}
	// try unique tileset palette
	sprintf(dest, "%s%s%s.pal", root, gfx_tileset_dir(), tileset);
	if (file_exists(dest)) { return; }
	// try unique tileset palette, trimming the suffix
	char name[FL_PATH_MAX] = {};
	remove_suffix(tileset, name);
	sprintf(dest, "%s%s%s.pal", root, gfx_tileset_dir(), name);
}
