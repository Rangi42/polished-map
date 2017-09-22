#ifndef CONFIG_H
#define CONFIG_H

#define NEW_BLK_NAME "NewMap.blk"

const char *gfx_tileset_dir(void);
bool project_path_from_blk_path(const char *blk_path, char *project_path);
void blk_path_from_project_path(const char *project_path, char *blk_path);
void palette_map_path(char *dest, const char *root, const char *tileset);
void tileset_path(char *dest, const char *root, const char *tileset);
void metatileset_path(char *dest, const char *root, const char *tileset);

#endif
