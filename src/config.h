#ifndef CONFIG_H
#define CONFIG_H

class Config {
private:
	static bool _monochrome, _256_tiles, _special_lighting, _roof_colors;
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
	static void map_headers_path(char *dest, const char *root);
	static void bg_tiles_pal_path(char *dest, const char *root);
	static void roofs_pal_path(char *dest, const char *root);
	inline static bool monochrome(void) { return _monochrome; }
	inline static void monochrome(bool m) { _monochrome = m; }
	inline static bool allow_256_tiles(void) { return _256_tiles; }
	inline static void allow_256_tiles(bool t) { _256_tiles = t; }
	inline static bool auto_load_special_lighting(void) { return _special_lighting; }
	inline static void auto_load_special_lighting(bool s) { _special_lighting = s; }
	inline static bool auto_load_roof_colors(void) { return _roof_colors; }
	inline static void auto_load_roof_colors(bool r) { _roof_colors = r; }
};

#endif
