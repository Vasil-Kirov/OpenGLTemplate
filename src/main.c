#include "gl/texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_video.h"
#include "log.h"
#include <SDL3/SDL.h>
#include <glad/glad.h>
#include "renderer.h"

const int WIDTH = 640;
const int HEIGHT = 480;

typedef struct {
	SDL_Window *window;
	int width;
	int height;
	SDL_GLContext gl_ctx;
} ProgramContext;

extern FILE *log_output;

void print_mat4(m4 m)
{
	VINFO("MATRIX");
	VINFO("%f %f %f %f", m.m[0][0], m.m[0][1], m.m[0][2], m.m[0][3]);
	VINFO("%f %f %f %f", m.m[1][0], m.m[1][1], m.m[1][2], m.m[1][3]);
	VINFO("%f %f %f %f", m.m[2][0], m.m[2][1], m.m[2][2], m.m[2][3]);
	VINFO("%f %f %f %f", m.m[3][0], m.m[3][1], m.m[3][2], m.m[3][3]);
}

int main(void)
{
	m4 start = M4(
		0, 2, 0, 0,
		-2, 0, 0, 0,
		0, 0, 2, 0,
		5, -3, 2, 1);
	m4 invert;
	inverse_matrix(start, &invert);
	print_mat4(start);
	print_mat4(invert);
	print_mat4(mat4_multiply(start, invert));
	

	fopen_s(&log_output, "log.txt", "w");

	if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
	{
		VFATL("Failed to initialize SDL: %s", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	ProgramContext ctx = {.width = WIDTH, .height = HEIGHT};


	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1 /* Enabled */);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

	ctx.window = SDL_CreateWindow("OpenGL", ctx.width, ctx.height, SDL_WINDOW_OPENGL);
	if(ctx.window == NULL)
	{
		VFATL("SDL_CreateWindow() failed: %s", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	ctx.gl_ctx = SDL_GL_CreateContext(ctx.window);
	if(ctx.gl_ctx == NULL)
	{
		VFATL("SDL_GL_CreateContext() failed: %s", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		VFATL("gladLoadGLLoader() failed: %s", SDL_GetError());
		SDL_GL_DestroyContext(ctx.gl_ctx);
		SDL_Quit();
		return 1;
	}

	VINFO("Vendor:   %s", glGetString(GL_VENDOR));
	VINFO("Renderer: %s", glGetString(GL_RENDERER));
	VINFO("Version:  %s", glGetString(GL_VERSION));

	Renderer renderer = make_renderer();
	if(renderer.initialized == false)
	{
		SDL_Quit();
		return 1;
	}

	AssetLoader asset_loader = make_asset_loader(STR_LIT("./assets/"));

	Asset vert_shader = load_asset(&asset_loader, STR_LIT("base.vert"));
	Asset frag_shader = load_asset(&asset_loader, STR_LIT("base.frag"));

	Asset test_image = load_image(&asset_loader, STR_LIT("opengl.png"));

	Image *img = as_img(test_image);
	Texture *test_tex = make_texture(&renderer, img->pixels, img->width, img->height, Filter_Linear);

	if(vert_shader.data == NULL || frag_shader.data == NULL)
	{
		SDL_Quit();
		return 1;
	}
	Shader *main_shader = compile_shader(&renderer, (Asset[]){vert_shader, frag_shader}, (ShaderType[]){ShaderType_Vertex, ShaderType_Pixel}, 2);
	if(!main_shader)
	{
		SDL_Quit();
		return 1;
	}

	free_asset(&asset_loader, &vert_shader);
	free_asset(&asset_loader, &frag_shader);

	Buffer *vert_buf = make_buffer(&renderer, BufferType_Vertex, KB(1));
	Buffer *index_buf = make_buffer(&renderer, BufferType_Index, KB(1));
	if(vert_buf == NULL || index_buf == NULL)
	{
		SDL_Quit();
		return 1;
	}

	Vertex *verts = map_buffer(&renderer, vert_buf);
	u32 *indexes = map_buffer(&renderer, index_buf);

	verts[0] = (Vertex) {.position = V3(-0.5, -0.5, 0), .color = V4(0, 0.5, 0.5, 1), .tex_coord = V2(0, 0)};
	verts[1] = (Vertex) {.position = V3(0.5, -0.5, 0),  .color = V4(0, 0.5, 0.5, 1), .tex_coord = V2(1, 0)};
	verts[2] = (Vertex) {.position = V3(0.5, 0.5, 0),   .color = V4(1, 0.5, 0.5, 1), .tex_coord = V2(1, 1)};
	verts[3] = (Vertex) {.position = V3(-0.5, 0.5, 0),  .color = V4(1, 0.5, 0.5, 1), .tex_coord = V2(0, 1)};
	indexes[0] = 0;
	indexes[1] = 1;
	indexes[2] = 2;
	indexes[3] = 0;
	indexes[4] = 2;
	indexes[5] = 3;
	unmap_buffer(&renderer, (void **)&verts, vert_buf);
	unmap_buffer(&renderer, (void **)&indexes, index_buf);

	set_shader_uniform_texture(&renderer, main_shader, "tex", test_tex, 0);

	SDL_GL_SetSwapInterval(1);
	set_shader(&renderer, main_shader);
	viewport(ctx.width, ctx.height);
	bool running = true;
	while(running)
	{
		clear_screen(&renderer, V4(0.047, 0.047, 0.047, 1.0));
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_EVENT_QUIT:
				{
					running = false;
				} break;
			}
		}

		draw_buffers(&renderer, vert_buf, index_buf, 6);
		SDL_GL_SwapWindow(ctx.window);
	}


	VINFO("Exiting...");
	SDL_GL_DestroyContext(ctx.gl_ctx);
	SDL_Quit();
	return 0;
}

