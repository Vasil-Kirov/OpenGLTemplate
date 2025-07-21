#pragma once

typedef enum {
	Filter_Linear,
	Filter_Nearest,
} FilterType;

typedef struct {
	unsigned int internal;
} Texture;

struct Renderer;

Texture *make_texture(struct Renderer *renderer, void *pixels, int width, int height, FilterType filter);

