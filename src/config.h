#ifndef CONFIG_H
#define CONFIG_H

#include <cstdlib>

class Config {
private:
	static bool _512_tiles, _0_before_1, _drag_and_drop;
	static bool _print_grid, _print_ids, _print_priority, _print_events, _print_warp_ids;
	static size_t _overworld_map_size;
public:
	static const char *gfx_tileset_dir(void);
	static const char *gfx_roof_dir(void);
	static bool project_path_from_blk_path(const char *blk_path, char *project_path);
	static void attributes_path(char *dest, const char *root, const char *tileset);
	static void tileset_path(char *dest, const char *root, const char *tileset);
	static bool tileset_before_path(char *dest, const char *root, const char *tileset);
	static bool tileset_after_path(char *dest, const char *root, const char *tileset);
	static void tileset_png_paths(char *dest, char *b_dest, char *a_dest, const char *root, const char *tileset);
	static void roof_path(char *dest, const char *root, const char *roof);
	static void roof_png_path(char *dest, const char *root, const char *roof);
	static void metatileset_path(char *dest, const char *root, const char *tileset);
	static bool collisions_path(char *dest, const char *root, const char *tileset);
	static void map_constants_path(char *dest, const char *root);
	static void tileset_constants_path(char *dest, const char *root);
	static void map_headers_path(char *dest, const char *root);
	static void event_script_path(char *dest, const char *root, const char *map_name);
	static void bg_tiles_pal_path(char *dest, const char *root);
	static void roofs_pal_path(char *dest, const char *root);
	static void special_pal_path(char *dest, const char *root, const char *filename, const char *landmark, const char *tileset);
	inline static bool allow_512_tiles(void) { return _512_tiles; }
	inline static void allow_512_tiles(bool t) { _512_tiles = t; }
	inline static bool arrange_0_before_1(void) { return _0_before_1; }
	inline static void arrange_0_before_1(bool b) { _0_before_1 = b; }
	inline static bool drag_and_drop(void) { return _drag_and_drop; }
	inline static void drag_and_drop(bool d) { _drag_and_drop = d; }
	inline static bool print_grid(void) { return _print_grid; }
	inline static void print_grid(bool p) { _print_grid = p; }
	inline static bool print_ids(void) { return _print_ids; }
	inline static void print_ids(bool p) { _print_ids = p; }
	inline static bool print_priority(void) { return _print_priority; }
	inline static void print_priority(bool p) { _print_priority = p; }
	inline static bool print_events(void) { return _print_events; }
	inline static void print_events(bool p) { _print_events = p; }
	inline static bool print_warp_ids(void) { return _print_warp_ids; }
	inline static void print_warp_ids(bool p) { _print_warp_ids = p; }
	inline static size_t overworld_map_size(void) { return _overworld_map_size; }
	inline static void overworld_map_size(size_t s) { _overworld_map_size = s; }
};

#endif
