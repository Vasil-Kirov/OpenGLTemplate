#include "renderer.h"
#include "log.h"
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void APIENTRY glDebugOutput(GLenum source, 
                            GLenum type, 
                            unsigned int id, 
                            GLenum severity, 
                            GLsizei length, 
                            const char *message, 
                            const void *userParam)
{
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    VERRO("---------------");
    VERRO("Debug message (%d): %s", id, message);

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             VINFO("Source: API"); break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   VINFO("Source: Window System"); break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: VINFO("Source: Shader Compiler"); break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     VINFO("Source: Third Party"); break;
        case GL_DEBUG_SOURCE_APPLICATION:     VINFO("Source: Application"); break;
        case GL_DEBUG_SOURCE_OTHER:           VINFO("Source: Other"); break;
    }

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               VINFO("Type: Error"); break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: VINFO("Type: Deprecated Behaviour"); break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  VINFO("Type: Undefined Behaviour"); break; 
        case GL_DEBUG_TYPE_PORTABILITY:         VINFO("Type: Portability"); break;
        case GL_DEBUG_TYPE_PERFORMANCE:         VINFO("Type: Performance"); break;
        case GL_DEBUG_TYPE_MARKER:              VINFO("Type: Marker"); break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          VINFO("Type: Push Group"); break;
        case GL_DEBUG_TYPE_POP_GROUP:           VINFO("Type: Pop Group"); break;
        case GL_DEBUG_TYPE_OTHER:               VINFO("Type: Other"); break;
    }
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         VINFO("Severity: high"); break;
        case GL_DEBUG_SEVERITY_MEDIUM:       VINFO("Severity: medium"); break;
        case GL_DEBUG_SEVERITY_LOW:          VINFO("Severity: low"); break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: VINFO("Severity: notification"); break;
    }
}

Renderer make_renderer()
{
	Renderer r = {};
	glCreateVertexArrays(1, &r.VAO);
	glEnable(GL_MULTISAMPLE);


	int gl_flags; glGetIntegerv(GL_CONTEXT_FLAGS, &gl_flags);
	if ((gl_flags & GL_CONTEXT_FLAG_DEBUG_BIT))
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
		glDebugMessageCallback(glDebugOutput, NULL);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	}

	r.state_allocator = make_alloc_arena(KB(1));
	if(r.state_allocator.start == NULL)
	{
		VFATL("Failed to create renderer memory allocator");
		return (Renderer){};
	}

	r.initialized = true;
	return r;
}

void set_renderer_target(Renderer *renderer, FrameBuffer *target)
{
	if(target == NULL)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, target->internal);
	}
}

void draw_buffers(Renderer *renderer, Buffer *vertex_buffer, Buffer *index_buffer, size_t count)
{
	assert(vertex_buffer->type == BufferType_Vertex);
	assert(index_buffer->type == BufferType_Index);
	glVertexArrayVertexBuffer(renderer->VAO, 0, vertex_buffer->internal, 0, sizeof(Vertex));
	glVertexArrayElementBuffer(renderer->VAO, index_buffer->internal);

	glEnableVertexArrayAttrib(renderer->VAO, 0);
	glVertexArrayAttribFormat(renderer->VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
	glVertexArrayAttribBinding(renderer->VAO, 0, 0);

	glEnableVertexArrayAttrib(renderer->VAO, 1);
	glVertexArrayAttribFormat(renderer->VAO, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, tex_coord));
	glVertexArrayAttribBinding(renderer->VAO, 1, 0);

	glEnableVertexArrayAttrib(renderer->VAO, 2);
	glVertexArrayAttribFormat(renderer->VAO, 2, 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, color));
	glVertexArrayAttribBinding(renderer->VAO, 2, 0);


	glBindVertexArray(renderer->VAO);
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}

void viewport(int width, int height)
{
	glViewport(0, 0, width, height);
}

void clear_screen(Renderer *renderer, v4 color)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(color.x, color.y, color.z, color.w);
}
