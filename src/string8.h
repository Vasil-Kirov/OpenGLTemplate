#pragma once


typedef struct {
	const char *data;
	size_t len;
} String8;


#define STR_LIT(literal) (String8){.data = literal, .len = sizeof(literal)-1}

