#ifndef CONFIG_H
#define CONFIG_H

const char *gfx_tileset_dir(void);
bool project_path_from_blk_path(const char *blk_path, char *project_path);
void palette_map_path(char *dest, const char *root, const char *tileset);
void tileset_path(char *dest, const char *root, const char *tileset);
void metatileset_path(char *dest, const char *root, const char *tileset);

#endif
