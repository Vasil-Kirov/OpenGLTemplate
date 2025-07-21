#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 vs_tex;
layout (location = 2) in vec4 vs_col;

out vec2 tex_coord;
out vec4 color;

void main()
{
	gl_Position = vec4(pos, 1.0);
	color = vs_col;
	tex_coord = vs_tex;
}

