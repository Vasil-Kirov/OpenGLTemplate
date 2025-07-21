#include "stb_image.h"
#include "asset_loader.h"
#include "log.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <SDL3/SDL.h>

Asset read_entire_file(char *file_name)
{
	SDL_IOStream *io = SDL_IOFromFile(file_name, "rb");
	if(io == NULL) return (Asset){};

	size_t size = SDL_GetIOSize(io);
	void *data = malloc(size);
	if(data != NULL)
	{
		if(SDL_ReadIO(io, data, size) == 0)
		{
			free(data);
			data = NULL;
		}
	}
	SDL_CloseIO(io);
	if(data == NULL) size = 0;
	return (Asset) {
		.type = AT_BinaryData,
		.data = data,
		.len = size,
	};
}

AssetLoader make_asset_loader(String8 asset_directory)
{
	return (AssetLoader) {
		.asset_directory = asset_directory,
		.alloc = make_alloc_libc(),
	};
}

char *get_asset_full_path(AssetLoader *loader, String8 asset_name)
{
	size_t full_path_len = asset_name.len + loader->asset_directory.len;
	char *full_path = new_array_t(char, full_path_len+1, &loader->alloc);
	if(full_path == NULL)
	{
		VERRO("loader->alloc(%zu) failed!", full_path_len);
		return NULL;
	}
	memcpy(full_path, loader->asset_directory.data, loader->asset_directory.len);
	memcpy(full_path+loader->asset_directory.len, asset_name.data, asset_name.len);
	full_path[full_path_len] = 0;

	return full_path;
}

Asset load_asset(AssetLoader *loader, String8 asset_name)
{
	char *full_path = get_asset_full_path(loader, asset_name);
	if(full_path == NULL)
	{
		return (Asset){};
	}

	Asset asset = read_entire_file(full_path);
	asset.path = full_path;

	return asset;
}

Asset load_image(AssetLoader *loader, String8 asset_name)
{
	Image *img = new_t(Image, &loader->alloc);
	if(img == NULL)
	{
		return (Asset){};
	}

	char *full_path = get_asset_full_path(loader, asset_name);
	if(full_path == NULL)
	{
		free_t(img, &loader->alloc);
		return (Asset){};
	}

	int channels;
	stbi_set_flip_vertically_on_load(1);
	img->pixels = stbi_load(full_path, &img->width, &img->height, &channels, 4);
	if(img->pixels == NULL)
	{
		free_t(img, &loader->alloc);
		free_t(full_path, &loader->alloc);
		return (Asset){};
	}

	Asset asset = {};
	asset.type = AT_Image;
	asset.path = full_path;
	asset.data = (char *)img;
	asset.len = sizeof(void*);

	return asset;
}

void free_asset(AssetLoader *loader, Asset *asset)
{
	if(asset->type == AT_Image && asset->data)
	{
		Image *img = (Image *)asset->data;

		assert(asset->len == sizeof(void*));
		assert(img->pixels);

		stbi_image_free(img->pixels);
	}

	free_t(asset->data, &loader->alloc);
	free_t(asset->path, &loader->alloc);
	asset->data = NULL;
}



