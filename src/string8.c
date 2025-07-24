#include "string8.h"
#include <string.h>

StringBuilder builder_make()
{
	StringBuilder b = {.alloc = make_alloc_libc(), .head = NULL};
	return b;
}

b8 builder_append(StringBuilder *builder, String8 str)
{
	String8 at = str;
	if(builder->head == NULL)
	{
		StringNode *new_node = new_t(StringNode, &builder->alloc);
		if(!new_node)
			return false;
		builder->head = new_node;
	}

	if(builder->head->at < NODE_BUF_SIZE)
	{
		size_t to_copy = min(at.len, NODE_BUF_SIZE-builder->head->at);
		memcpy(&builder->head->text[builder->head->at], at.data, to_copy);
		builder->head->at += to_copy;
		at.len -= to_copy;
		at.data += to_copy;

	}

	StringNode *prev = builder->head;
	StringNode *visit = builder->head->next;
	while(at.len > 0)
	{
		if(visit == NULL)
		{
			StringNode *new_node = new_t(StringNode, &builder->alloc);
			if(!new_node)
				return false;
			prev->next = new_node;
			visit = new_node;
		}

		size_t to_copy = min(at.len, NODE_BUF_SIZE-visit->at);
		memcpy(&visit->text[visit->at], at.data, to_copy);
		visit->at += to_copy;
		at.len -= to_copy;
		at.data += to_copy;

		prev  = prev->next;
		visit = visit->next;
	}

	return true;
}

String8 builder_finish(StringBuilder *builder, Allocator *alloc)
{
	if(builder->head == NULL)
	{
		char *empty = new_array_t(char, 1, alloc);
		if(empty == NULL)
			return STR_LIT("");
		empty[0] = '\0';
		return make_string(empty, 0);
	}

	size_t len = 0;
	StringNode *visit = builder->head;
	while(visit)
	{
		len += visit->at;
		visit = visit->next;
	}

	char *str = new_array_t(char, len+1, alloc);
	if(str == NULL)
		return STR_LIT("");

	size_t at = 0;
	visit = builder->head;
	while(visit)
	{
		StringNode *to_free = visit;

		memcpy(&str[at], visit->text, visit->at);
		at += visit->at;
		visit = visit->next;

		free_t(to_free, &builder->alloc);
	}
	str[len] = 0;
	return make_string(str, len);
}

inline void free_dyn_string(String8 str, Allocator *alloc)
{
	free_t((void *)str.data, alloc);
}

inline String8 make_string(const char *data, size_t len)
{
	return (String8){.data = data, .len = len};
}
