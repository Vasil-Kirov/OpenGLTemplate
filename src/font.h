#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include "base.h"
#include "renderer.h"
#include "vmath.h"

typedef struct {
	Texture *texture;
	iv2 size;
	iv2 bearing;
	u32 advance;
} Character;

typedef struct {
	Character *characters;
	int size;
} CharacterSet;

typedef struct {
	FT_Face face;
	Allocator alloc;
	CharacterSet *sets;
	int set_count;
	b8 initialized;
} Font;



