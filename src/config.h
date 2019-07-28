#ifndef CONFIG_H
#define CONFIG_H

class Config {
private:
	static bool _monochrome, _priority, _256_tiles, _drag_and_drop;
public:
	static const char *gfx_tileset_dir(void);
	static const char *gfx_roof_dir(void);
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
};

#endif
