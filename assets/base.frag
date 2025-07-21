#version 460 core

in vec2 tex_coord;
in vec4 color;

out vec4 frag_color;

uniform sampler2D tex;

void main()
{
	frag_color = color * texture(tex, tex_coord);
}

