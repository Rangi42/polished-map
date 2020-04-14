#ifndef CONFIG_H
#define CONFIG_H

#include <cstdlib>

class Config {
private:
	static bool _monochrome, _priority, _256_tiles, _drag_and_drop,
		_print_grid, _print_ids, _print_priority, _print_events;
	static size_t _overworld_map_size;
public:
	static void gfx_tileset_dir(char *dest, const char *root);
	static void gfx_roof_dir(char *dest, const char *root);
	static const char *palette_macro(void);
	static bool project_path_from_blk_path(const char *blk_path, char *project_path);
	static void palette_map_path(char *dest, const char *root, const char *tileset);
	static void tileset_path(char *dest, const char *root, const char *tileset);
	static void tileset_png_path(char *dest, const char *root, const char *tileset);
	static void roof_path(char *dest, const char *root, const char *roof);
	static void roof_png_path(char *dest, const char *root, const char *roof);
	static void metatileset_path(char *dest, const char *root, const char *tileset);
	static bool collisions_path(char *dest, const char *root, const char *tileset);
	static void map_constants_path(char *dest, const char *root);
	static void tileset_constants_path(char *dest, const char *root);
	static bool map_headers_path(char *dest, const char *root);
	static void map_header_path(char *dest, const char *root, const char *map_name);
	static void event_script_path(char *dest, const char *root, const char *map_name);
	static void bg_tiles_pal_path(char *dest, const char *root);
	static void roofs_pal_path(char *dest, const char *root);
	inline static bool monochrome(void) { return _monochrome; }
	inline static void monochrome(bool m) { _monochrome = m; }
	inline static bool allow_priority(void) { return _priority; }
	inline static void allow_priority(bool p) { _priority = p; }
	inline static bool allow_256_tiles(void) { return _256_tiles; }
	inline static void allow_256_tiles(bool t) { _256_tiles = t; }
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
	inline static size_t overworld_map_size(void) { return _overworld_map_size; }
	inline static void overworld_map_size(size_t s) { _overworld_map_size = s; }
};

#endif
