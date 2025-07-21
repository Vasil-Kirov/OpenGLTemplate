#include "texture.h"
#include <glad/glad.h>
#include <math.h>
#include "../renderer.h"
#include "../memory.h"

Texture *make_texture(Renderer *renderer, void *pixels, int width, int height, FilterType filter)
{
	GLint filter_type = 0;
	switch(filter)
	{
		case Filter_Linear:
		{
			filter_type = GL_LINEAR;
		} break;
		case Filter_Nearest:
		{
			filter_type = GL_NEAREST;
		} break;
	}


	GLuint tex;
	glCreateTextures(GL_TEXTURE_2D, 1, &tex);
	glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, filter_type);
	glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, filter_type);
	glTextureStorage2D(tex, 1, GL_RGBA8, width, height);
	glTextureSubImage2D(tex, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	Texture *result = new_t(Texture, &renderer->state_allocator);
	if(!result)
		return NULL;

	result->internal = tex;
	return result;
}

