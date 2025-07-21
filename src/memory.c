#include "base.h"
#include "memory.h"
#include "log.h"
#include <stdlib.h>
#include <string.h>


inline void *
align_16(void *address)
{
    return (void*)(((u64)(address) + 15ull) & 0xfffffffffffffff0);
}

void *arena_allocate(Allocator *arena, size_t size, b32 no_zero_out)
{
	arena->current = align_16(arena->current);
	void *result = arena->current;
	arena->current = (u8 *)arena->current + size;
	if((char *)arena->current >= (char *)arena->end)
	{
		size_t curr_offset = arena->current - arena->start;
		size_t prev_size = arena->end - arena->start;
		void *new_alloc = realloc(arena, prev_size * 1.5);
		if(new_alloc == NULL)
		{
			VFATL("realloc(%zu) failed!", (size_t)(prev_size * 1.5));
			return NULL;
		}
		arena->start = new_alloc;
		arena->current = arena->start + curr_offset;
	}

	if(!no_zero_out)
		memset(result, 0, size);
	return result;
}

void free_arena(Allocator *arena)
{
	free(arena->start);
	arena->start = NULL;
	arena->end = NULL;
	arena->current = NULL;
}

void *arena_fn(Allocator *this, AllocCMD cmd, size_t size, void *old_ptr, size_t old_size, const char *file_call, int line_no, const char *fn_call)
{
	void *result = NULL;
	switch(cmd)
	{
		case ACMD_Alloc:
		case ACMD_AllocNoZero:
		{
			result = arena_allocate(this, size, cmd == ACMD_AllocNoZero);
			if(result == NULL)
			{
				VFATL("Allocation failed! Called from %s:%d(%s)", file_call, line_no, fn_call);
			}
		} break;
		case ACMD_Free:
		{
			VERRO("Calling free on arena allocator that doesn't support it! The call was made from: %s:%d(%s)", file_call, line_no, fn_call);
		} break;
		case ACMD_Reset:
		{
			this->current = this->start;
		} break;
		case ACMD_Clear:
		{
			free_arena(this);
		} break;
	}

	return result;
}

Allocator make_alloc_arena(size_t size)
{
	Allocator res = {};
	res.start = malloc(size);
	if(res.start == NULL)
	{
		VERRO("malloc(%zu) failed when creating memory arena!", size);
		return (Allocator){};
	}
	
	res.end = (u8 *)res.start + size; 
	res.current = res.start;
	res.fn = arena_fn;
	return res;
}

void *libc_fn(Allocator *this, AllocCMD cmd, size_t size, void *old_ptr, size_t old_size, const char *file_call, int line_no, const char *fn_call)
{
	void *result = NULL;
	switch(cmd)
	{
		case ACMD_Alloc:
		case ACMD_AllocNoZero:
		{
			result = malloc(size);
			if(result == NULL)
			{
				VFATL("Allocation failed! Called from %s:%d(%s)", file_call, line_no, fn_call);
			}
			else if(cmd == ACMD_Alloc)
			{
				memset(result, 0, size);
			}
		} break;
		case ACMD_Free:
		{
			free(old_ptr);
		} break;
		case ACMD_Reset:
		{
			VERRO("Calling reset on libc allocator is invalid! Call from %s:%d(%s)", file_call, line_no, fn_call);
		} break;
		case ACMD_Clear:
		{
			VERRO("Calling clear on libc allocator is invalid! Call from %s:%d(%s)", file_call, line_no, fn_call);
		} break;
	}

	return result;
}

Allocator make_alloc_libc()
{
	Allocator res = {};
	res.fn = libc_fn;
	return res;
}

size_t align_to (int address, int alignment)
{
    return ((address + (alignment - 1)) & ~(alignment - 1));
}
