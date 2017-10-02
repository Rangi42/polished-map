#ifndef CONFIG_H
#define CONFIG_H

#pragma warning(push, 0)
#include <FL/Fl_Preferences.H>
#pragma warning(pop)

#define NEW_MAP_NAME "NewMap"

class Config {
public:
	enum Project { POKECRYSTAL, POKERED, POLISHED, PRISM };
private:
	static Fl_Preferences global_config;
	static Project global_project;
public:
	inline static Fl_Preferences config(void) { return global_config; }
	inline static Project project(void) { return global_project; }
	inline static void project(Project p) { global_project =  p; }
	static int get(const char *key, int default = 0);
	static void set(const char *key, int value);
	static const char *gfx_tileset_dir(void);
	static const char *map_macro(void);
	static bool project_path_from_blk_path(const char *blk_path, char *project_path);
	static void blk_path_from_project_path(const char *project_path, char *blk_path);
	static void palette_map_path(char *dest, const char *root, const char *tileset);
	static void tileset_path(char *dest, const char *root, const char *tileset);
	static void metatileset_path(char *dest, const char *root, const char *tileset);
	static void map_constants_path(char *dest, const char *root);
	static bool monochrome(void);
	static bool skip_tiles_60_to_7f(void);
};

#endif
