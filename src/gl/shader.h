#pragma once

#include "glad/glad.h"
#include "../asset_loader.h"
#include "../base.h"
#include "texture.h"

struct Renderer;

typedef enum {
	ShaderType_Vertex,
	ShaderType_Pixel,
	ShaderType_TessControl,
	ShaderType_TessEval,
	ShaderType_Geometry,
	ShaderType_Compute,
} ShaderType;

typedef struct {
	GLuint program;
} Shader;

Shader *compile_shader(struct Renderer *renderer, Asset *shaders, size_t shader_count);
void set_shader(struct Renderer *renderer, Shader *shader);
b8 set_shader_uniform_texture(struct Renderer *renderer, Shader *shader, const char *name, Texture *t, int unit);

