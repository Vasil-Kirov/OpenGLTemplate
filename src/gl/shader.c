#include "shader.h"
#include "../log.h"
#include "../renderer.h"
#include <stdbool.h>

GLuint compile_shader_component(struct Renderer *renderer, Asset *shader, ShaderType st)
{
	GLuint res = -1;
	switch(st)
	{
		case ShaderType_Vertex:
		{
			res = glCreateShader(GL_VERTEX_SHADER);
		} break;
		case ShaderType_Pixel:
		{
			res = glCreateShader(GL_FRAGMENT_SHADER);
		} break;
	}
	
	const char *data = shader->data;
	int len = shader->len;
	glShaderSource(res, 1, &data, &len);
	
	glCompileShader(res);
	
	int success;
	char info_log[512] = {0};
	glGetShaderiv(res, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(res, 512, NULL, info_log);
		VERRO("glCompileShader(%s) failed: %s", shader->path, info_log);
		return -1;
	}
	return res;
}

Shader *compile_shader(struct Renderer *renderer, Asset *vertex, Asset *fragment)
{
	GLuint vert = compile_shader_component(renderer, vertex, ShaderType_Vertex);
	GLuint frag = compile_shader_component(renderer, fragment, ShaderType_Pixel);
	if(vert == -1 || frag == -1) return NULL;

	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vert);
	glAttachShader(shader_program, frag);
	glLinkProgram(shader_program);
	glUseProgram(shader_program);
	
	int success;
	char info_log[512] = {0};
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(shader_program, 512, NULL, info_log);
		VERRO("Linking %s and %s failed: %s", vertex->path, fragment->path, info_log);
		return NULL;
	}
	glDetachShader(shader_program, vert);
	glDetachShader(shader_program, frag);
	glDeleteShader(vert);
	glDeleteShader(frag);

	Shader *shader = new_t(Shader, &renderer->state_allocator);
	if(shader == NULL)
		return NULL;

	shader->program = shader_program;
	return shader;
}

void set_shader(struct Renderer *renderer, Shader *shader)
{
	glUseProgram(shader->program);
}

b8 set_shader_uniform_texture(Renderer *renderer, Shader *shader, const char *name, Texture *t, int unit)
{
	GLint loc = glGetUniformLocation(shader->program, name);
	if(loc == -1) return false;
	glBindTextureUnit(unit, t->internal);
	glProgramUniform1i(shader->program, loc, unit);

	return true;
}

