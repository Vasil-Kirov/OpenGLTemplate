#include "buffer.h"
#include "../renderer.h"

Buffer *make_buffer(Renderer *renderer, BufferType type, size_t size)
{
	Buffer *buf = new_t(Buffer, &renderer->state_allocator);
	if(buf == NULL)
		return NULL;

	buf->type = type;
	buf->size = size;
	glCreateBuffers(1, &buf->internal);
	glNamedBufferStorage(buf->internal, size, NULL, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT);
	return buf;
}

void *map_buffer(struct Renderer *renderer, Buffer *buffer)
{
	return glMapNamedBuffer(buffer->internal, GL_READ_WRITE);
}

void unmap_buffer(struct Renderer *renderer, void **map, Buffer *buffer)
{
	*map = NULL;
	glUnmapNamedBuffer(buffer->internal);
}

