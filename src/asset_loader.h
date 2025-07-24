#pragma once
#include "base.h"
#include "memory.h"
#include "string8.h"

typedef struct {
	String8 asset_directory;
	Allocator alloc;
} AssetLoader;

typedef enum {
	AT_BinaryData,
	AT_Image,
} AssetType;

typedef struct {
	AssetType type;
	char *path;
	u8 *data;
	size_t len;
} Asset;

typedef struct {
	u8 *pixels;
	int width;
	int height;
} Image;

AssetLoader make_asset_loader(String8 asset_directory);
Asset load_asset(AssetLoader *loader, String8 asset_name);
Asset load_image(AssetLoader *loader, String8 asset_name);
void free_asset(AssetLoader *loader, Asset *asset);
Asset read_entire_file(const char *file_name);

#define as_img(asset) ((Image *)((asset).data))


