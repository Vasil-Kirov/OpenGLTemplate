#include "shader.h"
#include "../log.h"
#include "../renderer.h"
#include <stdbool.h>
#include <stdlib.h>

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
		case ShaderType_TessControl:
		{
			res = glCreateShader(GL_TESS_CONTROL_SHADER);
		} break;
		case ShaderType_TessEval:
		{
			res = glCreateShader(GL_TESS_EVALUATION_SHADER);
		} break;
		case ShaderType_Geometry:
		{
			res = glCreateShader(GL_GEOMETRY_SHADER);
		} break;
		case ShaderType_Compute:
		{
			res = glCreateShader(GL_COMPUTE_SHADER);
		} break;
	}
	
	const char *data = (const char *)shader->data;
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

Shader *compile_shader(struct Renderer *renderer, Asset *shader_files, ShaderType *types, size_t shader_count)
{
	GLuint *compiled_shaders = (GLuint *)malloc(sizeof(GLuint) * shader_count);
	if(compiled_shaders == NULL) return NULL;

	for(int i = 0; i < shader_count; ++i)
	{
		compiled_shaders[i] = compile_shader_component(renderer, &shader_files[i], types[i]);
		if(compiled_shaders[i] == -1) return NULL;
	}


	GLuint shader_program = glCreateProgram();
	for(int i = 0; i < shader_count; ++i)
	{
		glAttachShader(shader_program, compiled_shaders[i]);
	}

	glLinkProgram(shader_program);
	glUseProgram(shader_program);
	
	int success;
	char info_log[512] = {0};
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(shader_program, 512, NULL, info_log);
		VERRO("Linking shader program failed: %s", info_log);
		return NULL;
	}

	for(int i = 0; i < shader_count; ++i)
	{
		GLuint s = compiled_shaders[i];
		glDetachShader(shader_program, s);
		glDeleteShader(s);
	}
	free(compiled_shaders);

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

