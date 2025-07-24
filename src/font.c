#include "font.h"
#include "asset_loader.h"
#include "log.h"
#include "string8.h"
#include <assert.h>

FT_Library ft_lib = NULL;

Font load_font(String8 name)
{
	if(ft_lib == NULL)
	{
		if(FT_Init_FreeType(&ft_lib))
		{
			VERRO("FT_Init_FreeType failed!");
			return (Font){};
		}
	}

#ifdef _WIN32
#define PLATFORM_FONT_LOCATION "C:\\Windows\\Fonts\\"
#else
#error DEFINE_PLATFORM_FONT_LOCATION
#endif
	Font font = {};
	font.alloc = make_alloc_libc();

	StringBuilder b = builder_make();
	builder_append(&b, STR_LIT(PLATFORM_FONT_LOCATION));
	builder_append(&b, name);
	String8 path = builder_finish(&b, &font.alloc);

	Asset afont = read_entire_file(path.data);
	if(afont.data == NULL)
	{
		free_dyn_string(path, &font.alloc);
		return (Font){};
	}

	FT_Face face;
	if(FT_New_Memory_Face(ft_lib, (FT_Byte *)afont.data, afont.len, 0, &face))
	{
		free_dyn_string(path, &font.alloc);
		free(afont.data);
		VERRO("FT_New_Memory_Face failed!");
		return (Font){};
	}


	free_dyn_string(path, &font.alloc);
	font.face = face;
	font.initialized = true;
	font.set_count = 0;
	font.sets = new_array_t(CharacterSet, 64, &font.alloc);
	assert(font.sets);
	return font;
}

void font_load_glyph_size(Renderer *renderer, Font *font, int size)
{
	if(font->initialized == false)
		return;

	for(int i = 0; i < font->set_count; i += 1)
	{
		if(font->sets[i].size == size)
			return;
	}

	assert(font->set_count < 64);
	CharacterSet *set = &font->sets[font->set_count];
	set->characters = new_array_t(Character, 128, &font->alloc);
	assert(set->characters);



	FT_Set_Pixel_Sizes(font->face, 0, size);

	for(int char_idx = ' '; char_idx <= '~'; char_idx += 1)
	{
		if(FT_Load_Char(font->face, char_idx, FT_LOAD_RENDER))
		{
			VERRO("FT_Load_Char(%c) failed!", char_idx);
		}


		FT_GlyphSlot glyph = font->face->glyph;
		Texture *t = make_texture_ex(renderer, glyph->bitmap.buffer, glyph->bitmap.width, glyph->bitmap.rows, Filter_Linear, PixelLayout_R);
		if(t == NULL) continue;
		set->characters[char_idx - ' '] = (Character) {
			.texture = t,
			.size = IV2(glyph->bitmap.width, glyph->bitmap.rows),
			.bearing = IV2(glyph->bitmap_left, glyph->bitmap_top),
			.advance = glyph->advance.x,
		};
	}
}

