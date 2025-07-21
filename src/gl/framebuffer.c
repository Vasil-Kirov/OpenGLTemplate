#include "framebuffer.h"
#include "../renderer.h"

FrameBuffer *make_framebuffer(struct Renderer *renderer)
{
	FrameBuffer *fb = new_t(FrameBuffer, &renderer->state_allocator);
	if(fb == NULL)
		return NULL;

	glCreateFramebuffers(1, &fb->internal);
	return fb;
}

void set_framebuffer_texture(Renderer *renderer, FrameBuffer *fb, Texture *tex, FrameBufferTextureType type)
{
	GLenum attachment_translate[] = {
		GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7,
		GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT, GL_DEPTH_STENCIL_ATTACHMENT
	};

	GLenum attachment = attachment_translate[type];
	glNamedFramebufferTexture(fb->internal, attachment, tex->internal, 0);
}

