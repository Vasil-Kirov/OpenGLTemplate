#pragma once
#include <glad/glad.h>
#include "texture.h"

typedef struct {
	GLuint internal;
} FrameBuffer;

typedef enum {
	FBT_Color0,
	FBT_Color1,
	FBT_Color2,
	FBT_Color3,
	FBT_Color4,
	FBT_Color5,
	FBT_Color6,
	FBT_Color7,
	FBT_Depth,
	FBT_Stencil,
	FBT_DepthStencil,
} FrameBufferTextureType;

struct Renderer;

FrameBuffer *make_framebuffer(struct Renderer *renderer);
void set_framebuffer_texture(struct Renderer *renderer, FrameBuffer *fb, Texture *tex, FrameBufferTextureType type);
