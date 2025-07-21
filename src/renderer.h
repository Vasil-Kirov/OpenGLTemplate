#pragma once
#include "glad/glad.h"
#include "vmath.h"
#include "gl/buffer.h"
#include "gl/shader.h"
#include "gl/framebuffer.h"
#include "memory.h"

typedef struct Renderer {
	GLuint VAO;
	Allocator state_allocator;
	b8 initialized;
} Renderer;

typedef struct {
	v3 position;
	v2 tex_coord;
	v4 color;
} Vertex;

Renderer make_renderer();
void draw_buffers(Renderer *renderer, Buffer *vertex_buffer, Buffer *index_buffer, size_t count);
void viewport(int width, int height);
void clear_screen(Renderer *renderer, v4 color);
void set_renderer_target(Renderer *renderer, FrameBuffer *target);

