#ifndef CONFIG_H
#define CONFIG_H

void project_path_from_blk_path(char *path);
void palette_map_path(char *dest, const char *root, const char *tileset);
void tileset_path(char *dest, const char *root, const char *tileset);
void metatileset_path(char *dest, const char *root, const char *tileset);

#endif
