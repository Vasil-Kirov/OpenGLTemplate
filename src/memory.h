#pragma once
#include "base.h"

typedef struct Allocator Allocator;

typedef enum {
	ACMD_Alloc,
	ACMD_AllocNoZero,
	ACMD_Free,
	ACMD_Reset,
	ACMD_Clear,
} AllocCMD;

struct Allocator {
	void *start;
	void *end;
	void *current;
	void *(*fn)(Allocator *this, AllocCMD cmd, size_t size, void *old_ptr, size_t old_size, const char *file_call, int line_no, const char *fn_call);
};



Allocator make_alloc_arena(size_t size);
Allocator make_alloc_libc();

#define new_t(t, arena) (t *)(arena)->fn((arena), ACMD_Alloc, sizeof(t), NULL, 0, __FILE_NAME__, __LINE__, __func__)

#define new_array_t(t, count, arena) (t *)(arena)->fn((arena), ACMD_Alloc, sizeof(t) * (count), NULL, 0, __FILE_NAME__, __LINE__, __func__)

#define free_t(it, arena) (arena)->fn((arena), ACMD_Free, 0, it, 0, __FILE_NAME__, __LINE__, __func__)

