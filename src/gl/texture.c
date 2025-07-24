#include "texture.h"
#include <glad/glad.h>
#include <math.h>
#include "../renderer.h"
#include "../memory.h"

Texture *make_texture(Renderer *renderer, void *pixels, int width, int height, FilterType filter)
{
	return make_texture_ex(renderer, pixels, width, height, filter, PixelLayout_RGBA);
}

Texture *make_texture_ex(struct Renderer *renderer, void *pixels, int width, int height, FilterType filter, PixelLayout p_layout)
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

	GLenum format = 0;
	GLenum internal_format = 0;
	switch(p_layout)
	{
		case PixelLayout_R:
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 
			format = GL_R;
			internal_format = GL_R8;
		} break;
		case PixelLayout_RGBA:
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4); 
			format = GL_RGBA;
			internal_format = GL_RGBA8;
		} break;
	}


	GLuint tex;
	glCreateTextures(GL_TEXTURE_2D, 1, &tex);
	glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, filter_type);
	glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, filter_type);
	glTextureStorage2D(tex, 1, internal_format, width, height);
	glTextureSubImage2D(tex, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, pixels);

	Texture *result = new_t(Texture, &renderer->state_allocator);
	if(!result)
		return NULL;

	result->internal = tex;
	return result;
}

