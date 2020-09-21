#include <cstdio>
#include <cstring>

#pragma warning(push, 0)
#include <FL/filename.H>
#pragma warning(pop)

#include "utils.h"
#include "config.h"

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

bool Config::_monochrome = false, Config::_priority = false, Config::_256_tiles = false, Config::_drag_and_drop = true;
bool Config::_print_grid = false, Config::_print_ids = false, Config::_print_priority = false, Config::_print_events = false;

// wOverworldMapBlocks (aka OverworldMap or wOverworldMap) buffer size in WRAM
size_t Config::_overworld_map_size = 1300;

void Config::gfx_tileset_dir(char *dest, const char *root) {
	// try MAPDATA/ (source)
	sprintf(dest, "%s%s", root, "MAPDATA" DIR_SEP);
	if (file_exists(dest)) { return; }
	// last resort: gfx/tilesets/
	sprintf(dest, "%s%s", root, "gfx" DIR_SEP "tilesets" DIR_SEP);
}

void Config::gfx_roof_dir(char *dest, const char *root) {
	// gfx/tilesets/roofs/
	sprintf(dest, "%s%s", root, "gfx" DIR_SEP "tilesets" DIR_SEP "roofs" DIR_SEP);
}

const char *Config::palette_macro() {
	return "\ttilepal";
}

bool Config::project_path_from_blk_path(const char *blk_path, char *project_path) {
	char scratch_path[FL_PATH_MAX] = {};
	fl_filename_absolute(scratch_path, blk_path);
	char makefile[FL_PATH_MAX] = {};
	for (;;) {
		char *pivot = strrchr(scratch_path, *DIR_SEP);
		if (!pivot) { return false; }
		*pivot = '\0';
		// Make sure there's enough room for "/Makefile\0" (10 chars)
		if (pivot - scratch_path + 10 > FL_PATH_MAX) { return false; }
		strcpy(makefile, scratch_path);
		strcat(makefile, DIR_SEP "Makefile");
		if (!file_exists(makefile)) {
			strcpy(makefile, scratch_path);
			strcat(makefile, DIR_SEP "makefile");
		}
		if (file_exists(makefile)) { // the project directory contains a Makefile
			strcat(scratch_path, DIR_SEP);
			strcpy(project_path, scratch_path);
			return true;
		}
	}
}

void Config::palette_map_path(char *dest, const char *root, const char *tileset) {
	if (monochrome()) { return; }
	// try gfx/tilesets/*_palette_map.asm (pokecrystal)
	sprintf(dest, "%sgfx" DIR_SEP "tilesets" DIR_SEP "%s_palette_map.asm", root, tileset);
	if (file_exists(dest)) { return; }
	// try gfx/tilesets/*_palette_map.inc (SECTION-split pokecrystal)
	sprintf(dest, "%sgfx" DIR_SEP "tilesets" DIR_SEP "%s_palette_map.inc", root, tileset);
	if (file_exists(dest)) { return; }
	// try color/tilesets/*.asm (Red++ 3)
	char *name = trim_suffix(tileset);
	sprintf(dest, "%scolor" DIR_SEP "tilesets" DIR_SEP "%s.asm", root, name);
	free(name);
	if (file_exists(dest)) { return; }
	// last resort: tilesets/*_palette_map.asm (old pokecrystal)
	sprintf(dest, "%stilesets" DIR_SEP "%s_palette_map.asm", root, tileset);
}

void Config::tileset_path(char *dest, const char *root, const char *tileset) {
	char tileset_directory[FL_PATH_MAX] = {};
	gfx_tileset_dir(tileset_directory, root);
	// try gfx/tilesets/*.png (pokecrystal)
	sprintf(dest, "%s%s.png", tileset_directory, tileset);
	if (file_exists(dest)) { return; }
	// try gfx/tilesets/*.2bpp
	sprintf(dest, "%s%s.2bpp", tileset_directory, tileset);
	if (file_exists(dest)) { return; }
	// try MAPDATA/*.CHR (source)
	sprintf(dest, "%s%s.CHR", tileset_directory, tileset);
	if (file_exists(dest)) { return; }
	// last resort: gfx/tilesets/*.2bpp.lz
	sprintf(dest, "%s%s.2bpp.lz", tileset_directory, tileset);
}

void Config::tileset_png_path(char *dest, const char *root, const char *tileset) {
	char tileset_directory[FL_PATH_MAX] = {};
	gfx_tileset_dir(tileset_directory, root);
	sprintf(dest, "%s%s.png", tileset_directory, tileset);
}

void Config::roof_path(char *dest, const char *root, const char *roof) {
	char roof_directory[FL_PATH_MAX] = {};
	gfx_roof_dir(roof_directory, root);
	// try gfx/tilesets/roofs/*.png
	sprintf(dest, "%s%s.png", roof_directory, roof);
	if (file_exists(dest)) { return; }
	// try gfx/tilesets/roofs/*.2bpp
	sprintf(dest, "%s%s.2bpp", roof_directory, roof);
	if (file_exists(dest)) { return; }
	// try MAPDATA/*.CHR
	sprintf(dest, "%s%s.CHR", roof_directory, roof);
	if (file_exists(dest)) { return; }
	// last resort: gfx/tilesets/roofs/*.2bpp.lz
	sprintf(dest, "%s%s.2bpp.lz", roof_directory, roof);
}

void Config::roof_png_path(char *dest, const char *root, const char *roof) {
	char roof_directory[FL_PATH_MAX] = {};
	gfx_roof_dir(roof_directory, root);
	sprintf(dest, "%s%s.png", roof_directory, roof);
}

void Config::metatileset_path(char *dest, const char *root, const char *tileset) {
	// try data/tilesets/*_metatiles.bin (pokecrystal)
	sprintf(dest, "%sdata" DIR_SEP "tilesets" DIR_SEP "%s_metatiles.bin", root, tileset);
	if (file_exists(dest)) { return; }
	// try gfx/blocksets/*.bst (pokered)
	char *name = trim_suffix(tileset);
	sprintf(dest, "%sgfx" DIR_SEP "blocksets" DIR_SEP "%s.bst", root, name);
	free(name);
	if (file_exists(dest)) { return; }
	// try MAPDATA/*.CEL (source)
	sprintf(dest, "%sMAPDATA" DIR_SEP "%s.CEL", root, tileset);
	if (file_exists(dest)) { return; }
	// last resort: tilesets/*_metatiles.bin (old pokecrystal)
	sprintf(dest, "%stilesets" DIR_SEP "%s_metatiles.bin", root, tileset);
}

bool Config::collisions_path(char *dest, const char *root, const char *tileset) {
	// try data/tilesets/*_collision.asm (pokecrystal)
	sprintf(dest, "%sdata" DIR_SEP "tilesets" DIR_SEP "%s_collision.asm", root, tileset);
	if (file_exists(dest)) { return false; }
	// try data/tilesets/*_collision.inc (SECTION-split pokecrystal)
	sprintf(dest, "%sdata" DIR_SEP "tilesets" DIR_SEP "%s_collision.inc", root, tileset);
	if (file_exists(dest)) { return false; }
	// try data/tilesets/*_collision.bin (pokegold-spaceworld)
	sprintf(dest, "%sdata" DIR_SEP "tilesets" DIR_SEP "%s_collision.bin", root, tileset);
	if (file_exists(dest)) { return true; }
	// try tilesets/*_collision.asm (old pokecrystal, converted from .bin)
	sprintf(dest, "%stilesets" DIR_SEP "%s_collision.asm", root, tileset);
	if (file_exists(dest)) { return false; }
	// last resort: tilesets/*_collision.bin (old pokecrystal)
	sprintf(dest, "%stilesets" DIR_SEP "%s_collision.bin", root, tileset);
	return true;
}

void Config::map_constants_path(char *dest, const char *root) {
	// try constants/map_dimension_constants.asm (Prism)
	sprintf(dest, "%sconstants" DIR_SEP "map_dimension_constants.asm", root);
	if (file_exists(dest)) { return; }
	// try constants/map_constants.asm (pokecrystal, pokered)
	sprintf(dest, "%sconstants" DIR_SEP "map_constants.asm", root);
	if (file_exists(dest)) { return; }
	// last resort: constants/map_constants.inc (SECTION-split pokecrystal)
	sprintf(dest, "%sconstants" DIR_SEP "map_constants.inc", root);
}

bool Config::map_headers_path(char *dest, const char *root) {
	// try data/maps/maps.asm (pokecrystal)
	sprintf(dest, "%sdata" DIR_SEP "maps" DIR_SEP "maps.asm", root);
	if (file_exists(dest)) { return true; }
	// last resort: maps/map_headers.asm (old pokecrystal)
	sprintf(dest, "%smaps" DIR_SEP "map_headers.asm", root);
	return file_exists(dest);
}

void Config::map_header_path(char *dest, const char *root, const char *map_name) {
	// try data/maps/headers/%s.asm (pokered)
	sprintf(dest, "%sdata" DIR_SEP "maps" DIR_SEP "headers" DIR_SEP "%s.asm", root, map_name);
	if (file_exists(dest)) { return; }
	// last resort: data/mapHeaders/%s.asm (old pokered)
	sprintf(dest, "%sdata" DIR_SEP "mapHeaders" DIR_SEP "%s.asm", root, map_name);
}

void Config::event_script_path(char *dest, const char *root, const char *map_name) {
	// try maps/*.asm (pokecrystal, Prism)
	sprintf(dest, "%smaps" DIR_SEP "%s.asm", root, map_name);
	if (file_exists(dest)) { return; }
	// try SXY/%s.asm (source)
	sprintf(dest, "%sSXY" DIR_SEP "%s.asm", root, map_name);
	if (file_exists(dest)) { return; }
	// try data/maps/objects/%s.asm (pokered)
	sprintf(dest, "%sdata" DIR_SEP "maps" DIR_SEP "objects" DIR_SEP "%s.asm", root, map_name);
	if (file_exists(dest)) { return; }
	// last resort: data/mapObjects/%s.asm (old pokered)
	sprintf(dest, "%sdata" DIR_SEP "mapObjects" DIR_SEP "%s.asm", root, map_name);
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
