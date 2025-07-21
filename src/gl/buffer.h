#pragma once
#include "glad/glad.h"

typedef enum {
	BufferType_Vertex,
	BufferType_Index,
} BufferType;

typedef struct {
	GLuint internal;
	BufferType type;
	size_t size;
} Buffer;

struct Renderer;

Buffer *make_buffer(struct Renderer *renderer, BufferType type, size_t size);
void unmap_buffer(struct Renderer *renderer, void **map, Buffer *buffer);
void *map_buffer(struct Renderer *renderer, Buffer *buffer);

