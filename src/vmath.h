#pragma once

typedef struct __attribute__((packed)) {
	float x;
	float y;
} v2;

typedef struct __attribute__((packed)) {
	float x;
	float y;
	float z;
} v3;

typedef struct {
	float x;
	float y;
	float z;
	float w;
} v4;


static inline v2 V2(float x, float y)
{
	return (v2){x, y};
}

static inline v3 V3(float x, float y, float z)
{
	return (v3){x, y, z};
}

static inline v4 V4(float x, float y, float z, float w)
{
	return (v4){x, y, z, w};
}

