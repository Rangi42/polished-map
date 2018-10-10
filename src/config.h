#ifndef CONFIG_H
#define CONFIG_H

#pragma warning(push, 0)
#include <FL/Fl_Preferences.H>
#pragma warning(pop)

#define NEW_MAP_NAME "New Map"

class Config {
private:
	static Fl_Preferences global_config;
	static bool _monochrome, _256_tiles;
public:
	inline static Fl_Preferences config(void) { return global_config; }
	static int get(const char *key, int default_ = 0);
	static void set(const char *key, int value);
	static const char *gfx_tileset_dir(void);
	static const char *palette_macro(void);
	static bool project_path_from_blk_path(const char *blk_path, char *project_path);
	static void palette_map_path(char *dest, const char *root, const char *tileset);
	static void tileset_path(char *dest, const char *root, const char *tileset);
	static void tileset_png_path(char *dest, const char *root, const char *tileset);
	static void metatileset_path(char *dest, const char *root, const char *tileset);
	static void map_constants_path(char *dest, const char *root);
	static void tileset_constants_path(char *dest, const char *root);
	static void map_headers_path(char *dest, const char *root);
	static void bg_tiles_pal_path(char *dest, const char *root);
	inline static bool monochrome(void) { return _monochrome; }
	inline static void monochrome(bool m) { _monochrome = m; }
	inline static bool allow_256_tiles(void) { return _256_tiles; }
	inline static void allow_256_tiles(bool t) { _256_tiles = t; }
};

#endif
