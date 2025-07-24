#pragma once

typedef enum {
	Filter_Linear,
	Filter_Nearest,
} FilterType;

typedef enum {
	PixelLayout_RGBA,
	PixelLayout_R,
} PixelLayout;

typedef struct {
	unsigned int internal;
} Texture;

struct Renderer;

Texture *make_texture(struct Renderer *renderer, void *pixels, int width, int height, FilterType filter);

Texture *make_texture_ex(struct Renderer *renderer, void *pixels, int width, int height, FilterType filter, PixelLayout p_layout);

