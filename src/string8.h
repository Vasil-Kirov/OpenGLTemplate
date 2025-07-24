#pragma once

#include "memory.h"
#include "base.h"

typedef struct {
	const char *data;
	size_t len;
} String8;

#define NODE_BUF_SIZE 128
typedef struct StringNode StringNode;

struct StringNode {
	char text[NODE_BUF_SIZE];
	int at;
	StringNode *next;
};

typedef struct {
	StringNode *head;
	Allocator alloc;
} StringBuilder;

#define STR_LIT(literal) (String8){.data = literal, .len = sizeof(literal)-1}


StringBuilder builder_make();
b8 builder_append(StringBuilder *builder, String8 str);
String8 builder_finish(StringBuilder *builder, Allocator *alloc);

String8 make_string(const char *data, size_t len);
void free_dyn_string(String8 str, Allocator *alloc);



