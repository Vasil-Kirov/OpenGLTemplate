#pragma once
#include "base.h"

#if defined(__cplusplus)
extern "C"
{
#endif


#include <immintrin.h>
#include <math.h>
#include <stdarg.h>

#define MAX_BONES_PER 4
#define SQUARE(x) ((x) * (x))
#define PI				3.14159265358979323846

#pragma pack(push, 1)

	typedef struct _iv2
	{
		i32 x;
		i32 y;
	} iv2;

	typedef struct _v2
	{
		f32 x;
		f32 y;
	} v2;

	typedef struct _v3
	{
		f32 x;
		f32 y;
		f32 z;
	} v3;

	typedef union _v4
	{
		struct {
			f32 x;
			f32 y;
			f32 z;
			f32 w;
		};
		__m128 simd;
	} __attribute__((aligned(16))) v4;

	typedef struct _m2
	{
		f32 x1;
		f32 y1;
		f32 x2;
		f32 y2;
	} m2;
	typedef struct _m3
	{
		f32 m[3][3];
	} m3;
	typedef struct _m4
	{
		f32 m[4][4];
	} m4;
	typedef struct _m34
	{
		f32 m[3][4];
	} m34;
	typedef struct mesh_vertex
	{
		v3 position;
		v3 normal;
		v2 texture;
		int bone_ids[MAX_BONES_PER];
		float weights[MAX_BONES_PER];
	} mesh_vertex;

	typedef struct Mesh
	{
		mesh_vertex *vertexes;
		uint32_t *indexes;
		int vert_count;
		int index_count;
	} Mesh;

	typedef struct Bone_Info 
	{
		int id;
		m4 transform;
	} Bone_Info;
#pragma pack(pop)

typedef union quaternion
{
	struct {
		f32 x;
		f32 y;
		f32 z;
		f32 w;
	};
	f32 v[4];
} quaternion;

typedef struct camera
{
	v3 position;
	v3 look_dir;
	float yaw;
	float pitch;
	double mouse_x;
	double mouse_y;
	b32 is_locked;
} camera;


static inline iv2 IV2(i32 x, i32 y)
{
	return (iv2){x, y};
}

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



inline double
deg_to_rad(f32 degrees)
{
	return (double)(degrees * (PI / 180));
}
inline double
rad_to_deg(f32 Rad)
{
	return (double)(Rad * (180.0f / PI));
}

inline m4
x_rotation(f32 Angle)
{
	f32 c = cos(Angle);
	f32 s = sin(Angle);
	m4 R =
		{
			{{1, 0, 0, 0},
			 {0, c, -s, 0},
			 {0, s, c, 0},
			 {0, 0, 0, 1}}};
	return R;
}

inline m4
y_rotation(f32 Angle)
{
	f32 c = cos(Angle);
	f32 s = sin(Angle);
	m4 R =
		{
			{{c, 0, s, 0},
			 {0, 1, 0, 0},
			 {-s, 0, c, 0},
			 {0, 0, 0, 1}}};
	return R;
}

inline m4
z_rotation(f32 Angle)
{
	f32 c = cosf(Angle);
	f32 s = sinf(Angle);
	m4 R =
		{
			{{c, -s, 0, 0},
			 {s, c, 0, 0},
			 {0, 0, 1, 0},
			 {0, 0, 0, 1}}};
	return R;
}

inline f32
lerp(f32 a, f32 b, f32 t)
{
	return a + (b - a) * t;
}

inline f64
lerp64(f64 a, f64 b, f64 t)
{
	return a + (b - a) * t;
}

inline f64
inv_lerp64(f64 a, f64 b, f64 v)
{
	return (v - a) / (b - a);
}

inline v3
vector_lerp(v3 vector_a, v3 vector_b, double t)
{
	v3 result;
	result.x = lerp64(vector_a.x, vector_b.x, t);
	result.y = lerp64(vector_a.y, vector_b.y, t);
	result.z = lerp64(vector_a.z, vector_b.z, t);
	return result;
}

inline m4
matrix_lerp(m4 matrix_a, m4 matrix_b, double t)
{
	m4 result;
	for(int y = 0; y < 4; ++y)
	{
		for(int x = 0; x < 4; ++x)
		{
			result.m[y][x] = lerp64(matrix_a.m[y][x], matrix_b.m[y][x], t);
		}
	}
	return result;
}

inline m4
transpose(m4 A)
{
	m4 R = {};
	for (int j = 0; j <= 3; ++j)
	{
		for (int i = 0; i <= 3; ++i)
		{
			R.m[j][i] = A.m[i][j];
		}
	}
	return R;
}

inline m4
projection(f32 WidthOverHeight, f32 FOV)
{
	f32 Near = 1.0f;
	f32 Far = 100.0f;

	f32 f = 1.0f / tanf(deg_to_rad(FOV / 2.0f));
	f32 fn = 1.0f / (Near - Far);

	f32 a = f / WidthOverHeight;
	f32 b = f;
	f32 c = Far * fn;
	f32 d = Near * Far * fn;

	m4 Result =
		{
			{{a, 0, 0, 0},
			 {0, b, 0, 0},
			 {0, 0, c, -1},
			 {0, 0, d, 0}}};
	return Result;
}

inline m4
mat4_multiply(m4 A, m4 B)
{
	m4 Ret;

	for (unsigned int i = 0; i < 4; i++)
	{
		for (unsigned int j = 0; j < 4; j++)
		{
			Ret.m[i][j] = A.m[i][0] * B.m[0][j] +
						  A.m[i][1] * B.m[1][j] +
						  A.m[i][2] * B.m[2][j] +
						  A.m[i][3] * B.m[3][j];
		}
	}

	return Ret;
}

inline m4
mat4_multiply_multiple(int number, m4 A, m4 B, ...)
{
	va_list args;
	va_start(args, B);

	m4 Result = mat4_multiply(A, B);
	if (number <= 2)
	{
		return Result;
	}
	for (int i = 2; i < number; ++i)
	{
		m4 Right = va_arg(args, m4);
		Result = mat4_multiply(Result, Right);
	}
	return Result;
}

// NOTE(Vasko): I wrote this comment bellow a couple of months ago and I don't know what it means
/* REQUIRES TRANSPOSED MATRIX!!! */
inline v3
mat4_v3_multiply(m4 A, v3 B)
{
	v3 Ret;
	float w = 1.0f;

	Ret.x = B.x * A.m[0][0] + B.y * A.m[1][0] + B.z * A.m[2][0] + w * A.m[3][0];
	Ret.y = B.x * A.m[0][1] + B.y * A.m[1][1] + B.z * A.m[2][1] + w * A.m[3][1];
	Ret.z = B.x * A.m[0][2] + B.y * A.m[1][2] + B.z * A.m[2][2] + w * A.m[3][2];

	return Ret;
}

inline f32
map_in_range(f32 x, f32 minx, f32 maxx, f32 a, f32 b)
{
	f32 Result = ((b - a) * ((x - minx) / (maxx - minx)) + a);
	return Result;
}

#if 0
inline float
normalize_coordinate(float x, float maxx, float minx)
{
	float Result;
	Result = (2.0f * ((x - minx) / (maxx - minx))) - 1.0f;
	return Result;
}
#endif

inline v3
v3_map_in_range(v3 target, float minx, float maxx, float from, float to)
{
	v3 result = {};
	result.x = map_in_range(target.x, minx, maxx, from, to);
	result.y = map_in_range(target.y, minx, maxx, from, to);
	result.z = map_in_range(target.z, minx, maxx, from, to);
	return result;
}

inline v4
v4_map_in_range(v4 target, float minx, float maxx, float from, float to)
{
	v4 result = {};
	result.x = map_in_range(target.x, minx, maxx, from, to);
	result.y = map_in_range(target.y, minx, maxx, from, to);
	result.z = map_in_range(target.z, minx, maxx, from, to);
	result.w = map_in_range(target.w, minx, maxx, from, to);
	return result;
}

inline v4
mat4_v4_multiply(m4 A, v4 B)
{
	v4 Ret;

	Ret.x = B.x * A.m[0][0] + B.y * A.m[1][0] + B.z * A.m[2][0] + B.w * A.m[3][0];
	Ret.y = B.x * A.m[0][1] + B.y * A.m[1][1] + B.z * A.m[2][1] + B.w * A.m[3][1];
	Ret.z = B.x * A.m[0][2] + B.y * A.m[1][2] + B.z * A.m[2][2] + B.w * A.m[3][2];
	Ret.w = B.x * A.m[0][3] + B.y * A.m[1][3] + B.z * A.m[2][3] + B.w * A.m[3][3];

	return Ret;
}

inline m4
M4(
	float num1, float num2, float num3, float num4,
	float num5, float num6, float num7, float num8,
	float num9, float num10, float num11, float num12,
	float num13, float num14, float num15, float num16)
{
	m4 matrix = {};
	matrix.m[0][0] = num1;
	matrix.m[0][1] = num2;
	matrix.m[0][2] = num3;
	matrix.m[0][3] = num4;
	matrix.m[1][0] = num5;
	matrix.m[1][1] = num6;
	matrix.m[1][2] = num7;
	matrix.m[1][3] = num8;
	matrix.m[2][0] = num9;
	matrix.m[2][1] = num10;
	matrix.m[2][2] = num11;
	matrix.m[2][3] = num12;
	matrix.m[3][0] = num13;
	matrix.m[3][1] = num14;
	matrix.m[3][2] = num15;
	matrix.m[3][3] = num16;
	return matrix;
}

inline m4
identity()
{
	m4 Result = {};
	Result = M4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	return Result;
}

inline f64
double_map_in_range(f64 x, f64 minx, f64 maxx, f64 a, f64 b)
{
	f64 Result = ((b - a) * ((x - minx) / (maxx - minx)) + a);
	return Result;
}

inline float
v3_distance_squared(v3 pa, v3 pb)
{
	float x = pb.x - pa.x;
	float y = pb.y - pa.y;
	float z = pb.z - pa.z;
	f32 c = SQUARE(x) + SQUARE(y) + SQUARE(z);

	return c;
}

inline float
v3_distance(v3 pa, v3 pb)
{
	f32 c = sqrtf(v3_distance_squared(pa, pb));

	return c;
}

inline b32
v3_check_equality(v3 A, v3 B)
{
	return (A.x == B.x && A.y == B.y && A.z == B.z);
}

inline v3
v3_cross(v3 A, v3 B)
{
	v3 R = {};
	R.x = A.y * B.z - A.z * B.y;
	R.y = A.z * B.x - A.x * B.z;
	R.z = A.x * B.y - A.y * B.x;
	return R;
}

inline f32
v3_get_length(v3 A)
{
	return (sqrtf(A.x * A.x + A.y * A.y + A.z * A.z));
}

inline v3
v3_normalize(v3 A)
{
	f32 length = v3_get_length(A);

	A.x /= length;
	A.y /= length;
	A.z /= length;

	return A;
}

inline v3
v3_add(v3 A, v3 B)
{
	v3 R = {};
	R.x = A.x + B.x;
	R.y = A.y + B.y;
	R.z = A.z + B.z;
	return R;
}

inline v3
v3_sub(v3 A, v3 B)
{
	v3 R = {};
	R.x = A.x - B.x;
	R.y = A.y - B.y;
	R.z = A.z - B.z;
	return R;
}

inline v3
v3_scale(v3 A, f32 B)
{
	v3 R = {};
	R.x = A.x * B;
	R.y = A.y * B;
	R.z = A.z * B;
	return R;
}

inline v3
v3_f32_add(v3 A, f32 B)
{
	v3 R = {};
	R.x = A.x + B;
	R.y = A.y + B;
	R.z = A.z + B;
	return R;
}

inline v3
v3_negate(v3 A)
{
	return (v3){-A.x, -A.y, -A.z};
}

inline v3
v3_v3_scale(v3 A, v3 B)
{
	return (v3){A.x * B.x, A.y * B.y, A.z * B.z};
}

inline f32
v3_dot(v3 A, v3 B)
{
	return (A.x * B.x + A.y * B.y + A.z * B.z);
}

inline v4
v4_scale(v4 vector, f32 scalar)
{
	v4 R = {};
	R.x = vector.x * scalar;
	R.y = vector.y * scalar;
	R.z = vector.z * scalar;
	R.w = vector.w * scalar;
	return R;
}

inline f32
v4_dot(v4 A, v4 B)
{
	return (A.x * B.x + A.y * B.y + A.z * B.z + A.w * B.w);
}

inline f32
v4_len(v4 vec)
{
	return sqrtf(vec.w * vec.w + vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

inline v4
v4_normalize(v4 vec)
{
	f32 len = v4_len(vec);
	vec.x /= len;
	vec.y /= len;
	vec.z /= len;
	vec.w /= len;
	return vec;
}

inline m4
mat4_v3_scale(m4 matrix, v3 scalar)
{
	m4 result;
	*(v4 *)result.m[0] = v4_scale(*(v4 *)matrix.m[0], scalar.x);
	*(v4 *)result.m[1] = v4_scale(*(v4 *)matrix.m[1], scalar.y);
	*(v4 *)result.m[2] = v4_scale(*(v4 *)matrix.m[2], scalar.z);
	*(v4 *)result.m[3] = *(v4 *)matrix.m[3];
	return result;
}

inline m3
quaternion_to_m3(quaternion q0)
{
	m3 result;

	f32 qx_square2 = 2.0 * SQUARE(q0.x);
	f32 qy_square2 = 2.0 * SQUARE(q0.y);
	f32 qz_square2 = 2.0 * SQUARE(q0.z);
	f32 qz_qw2 = 2.0 * q0.z * q0.w;
	f32 qx_qy2 = 2.0 * q0.x * q0.y;
	f32 qx_qz2 = 2.0 * q0.x * q0.z;
	f32 qy_qw2 = 2.0 * q0.y * q0.w;
	f32 qy_qz2 = 2.0 * q0.y * q0.z;
	f32 qx_qw2 = 2.0 * q0.x * q0.w;

	result.m[0][0] = 1.0 - qy_square2 - qz_square2;
	result.m[0][1] = qx_qy2 - qz_qw2;
	result.m[0][2] = qx_qz2 + qy_qw2;

	result.m[1][0] = qx_qy2 + qz_qw2;
	result.m[1][1] = 1 - qx_square2 - qz_square2;
	result.m[1][2] = qy_qz2 - qx_qw2;

	result.m[2][0] = qx_qz2 - qy_qw2;
	result.m[2][1] = qy_qz2 + qx_qw2;
	result.m[2][2] = 1 - qx_square2 - qy_square2;
	return result;
}

inline m4
quaternion_to_m4(quaternion q0)
{
	m4 result;
	m3 to_conv = quaternion_to_m3(q0);		
	for(int y = 0; y < 3; ++y)
	{
		for(int x = 0; x < 3; ++x)
		{
			result.m[y][x] = to_conv.m[y][x];
		}
	}
	for(int y = 0; y < 4; ++y)
	{
		result.m[y][3] = 0;
	}
	__m128 zeros = _mm_set1_ps(0);
	*(__m128 *)result.m[3] = zeros;
	result.m[3][3] = 1;
	return result;
}

inline m4
point_at(v3 pos, v3 target, v3 up)
{
	v3 new_forward = v3_sub(target, pos);
	new_forward = v3_normalize(new_forward);

	v3 a = v3_scale(new_forward, v3_dot(up, new_forward));
	v3 new_up = v3_sub(up, a);
	new_up = v3_normalize(new_up);

	v3 new_right = v3_cross(new_up, new_forward);

	m4 matrix = M4(
		new_right.x, new_right.y, new_right.z, 0.0f,
		new_up.x, new_up.y, new_up.z, 0.0f,
		new_forward.x, new_forward.y, new_forward.z, 0.0f,
		pos.x, pos.y, pos.z, 0.0f);
	return matrix;
}

inline m4
translate(v3 position)
{
	return M4(
		1.0f, 0.0f, 0.0f, position.x,
		0.0f, 1.0f, 0.0f, position.y,
		0.0f, 0.0f, 1.0f, position.z,
		0.0f, 0.0f, 0.0f, 1.0f);
}

// No idea
inline quaternion
slerp(quaternion q0, quaternion q1, double t)
{
	f32 dot = v4_dot(*(v4 *)q0.v, *(v4 *)q1.v);

	f32 abs_dot = dot;
	if(dot < 0.0)
		abs_dot = -dot;
	if(abs_dot >= 1.0)
	{
		return q1;
	}
	double half_theta = acos(dot);
	double sin_half_theta = sqrt(1.0 - dot * dot);
	double abs_sin_half_theta = sin_half_theta;
	if(sin_half_theta < 0.0)
		abs_sin_half_theta = -sin_half_theta;

	if(abs_sin_half_theta < 0.001)
	{
		quaternion result;
		result.w = (q0.w * 0.5 + q1.w * 0.5);
		result.x = (q0.x * 0.5 + q1.x * 0.5);
		result.y = (q0.y * 0.5 + q1.y * 0.5);
		result.z = (q0.z * 0.5 + q1.z * 0.5);
		return result;	
	}
	double ratio_a = sin((1 - t) * half_theta) / sin_half_theta;
	double ratio_b = sin(t * half_theta) / sin_half_theta; 

	quaternion result;
	result.w = (q0.w * ratio_a + q1.w * ratio_b);
	result.x = (q0.x * ratio_a + q1.x * ratio_b);
	result.y = (q0.y * ratio_a + q1.y * ratio_b);
	result.z = (q0.z * ratio_a + q1.z * ratio_b);
	return result;
}


inline m4
quick_inverse(m4 m)
{
	m4 matrix;
	matrix.m[0][0] = m.m[0][0];
	matrix.m[0][1] = m.m[1][0];
	matrix.m[0][2] = m.m[2][0];
	matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = m.m[0][1];
	matrix.m[1][1] = m.m[1][1];
	matrix.m[1][2] = m.m[2][1];
	matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = m.m[0][2];
	matrix.m[2][1] = m.m[1][2];
	matrix.m[2][2] = m.m[2][2];
	matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
	matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
	matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

#if 0
/* Calculates the view matrix and gives it back in column-major order */
inline m4
calculate_view_matrix(camera *cm)
{
	v3 up = {0.0f, 1.0f, 0.0f};
	v3 target = {0.0f, 0.0f, 1.0f};
	m4 camera_y_rotation = y_rotation(cm->yaw);
	m4 camera_x_rotation = x_rotation(cm->pitch);
	m4 camera_rotation = mat4_multiply(camera_x_rotation, camera_y_rotation);
	cm->look_dir = mat4_v3_multiply(camera_rotation, target);
	target = v3_add(cm->position, cm->look_dir);

	m4 camera = point_at(cm->position, target, up);
	return transpose(quick_inverse(camera));
}
#endif

inline v4
create_v4(f32 a, f32 b, f32 c, f32 d)
{
	v4 result = {a, b, c, d};
	return result;
}

// NOTE(Vasko): row counted from 0
inline void
add_row_from_v4(m4 *matrix, v4 vector, int row)
{
	matrix->m[row][0] = vector.x;
	matrix->m[row][1] = vector.y;
	matrix->m[row][2] = vector.z;
	matrix->m[row][3] = vector.w;
}
inline void
add_row_from_f32_arr(m4 *matrix, f32 *vector, int row)
{
	matrix->m[row][0] = vector[0];
	matrix->m[row][1] = vector[1];
	matrix->m[row][2] = vector[2];
	matrix->m[row][3] = vector[3];
}

inline b32
inverse_matrix(m4 input, m4 *output)
{
	double det, inv[16];
	float *m = &(input.m[0][0]);

	inv[0] = m[5] * m[10] * m[15] -
			 m[5] * m[11] * m[14] -
			 m[9] * m[6] * m[15] +
			 m[9] * m[7] * m[14] +
			 m[13] * m[6] * m[11] -
			 m[13] * m[7] * m[10];

	inv[4] = -m[4] * m[10] * m[15] +
			 m[4] * m[11] * m[14] +
			 m[8] * m[6] * m[15] -
			 m[8] * m[7] * m[14] -
			 m[12] * m[6] * m[11] +
			 m[12] * m[7] * m[10];

	inv[8] = m[4] * m[9] * m[15] -
			 m[4] * m[11] * m[13] -
			 m[8] * m[5] * m[15] +
			 m[8] * m[7] * m[13] +
			 m[12] * m[5] * m[11] -
			 m[12] * m[7] * m[9];

	inv[12] = -m[4] * m[9] * m[14] +
			  m[4] * m[10] * m[13] +
			  m[8] * m[5] * m[14] -
			  m[8] * m[6] * m[13] -
			  m[12] * m[5] * m[10] +
			  m[12] * m[6] * m[9];

	inv[1] = -m[1] * m[10] * m[15] +
			 m[1] * m[11] * m[14] +
			 m[9] * m[2] * m[15] -
			 m[9] * m[3] * m[14] -
			 m[13] * m[2] * m[11] +
			 m[13] * m[3] * m[10];

	inv[5] = m[0] * m[10] * m[15] -
			 m[0] * m[11] * m[14] -
			 m[8] * m[2] * m[15] +
			 m[8] * m[3] * m[14] +
			 m[12] * m[2] * m[11] -
			 m[12] * m[3] * m[10];

	inv[9] = -m[0] * m[9] * m[15] +
			 m[0] * m[11] * m[13] +
			 m[8] * m[1] * m[15] -
			 m[8] * m[3] * m[13] -
			 m[12] * m[1] * m[11] +
			 m[12] * m[3] * m[9];

	inv[13] = m[0] * m[9] * m[14] -
			  m[0] * m[10] * m[13] -
			  m[8] * m[1] * m[14] +
			  m[8] * m[2] * m[13] +
			  m[12] * m[1] * m[10] -
			  m[12] * m[2] * m[9];

	inv[2] = m[1] * m[6] * m[15] -
			 m[1] * m[7] * m[14] -
			 m[5] * m[2] * m[15] +
			 m[5] * m[3] * m[14] +
			 m[13] * m[2] * m[7] -
			 m[13] * m[3] * m[6];

	inv[6] = -m[0] * m[6] * m[15] +
			 m[0] * m[7] * m[14] +
			 m[4] * m[2] * m[15] -
			 m[4] * m[3] * m[14] -
			 m[12] * m[2] * m[7] +
			 m[12] * m[3] * m[6];

	inv[10] = m[0] * m[5] * m[15] -
			  m[0] * m[7] * m[13] -
			  m[4] * m[1] * m[15] +
			  m[4] * m[3] * m[13] +
			  m[12] * m[1] * m[7] -
			  m[12] * m[3] * m[5];

	inv[14] = -m[0] * m[5] * m[14] +
			  m[0] * m[6] * m[13] +
			  m[4] * m[1] * m[14] -
			  m[4] * m[2] * m[13] -
			  m[12] * m[1] * m[6] +
			  m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] +
			 m[1] * m[7] * m[10] +
			 m[5] * m[2] * m[11] -
			 m[5] * m[3] * m[10] -
			 m[9] * m[2] * m[7] +
			 m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] -
			 m[0] * m[7] * m[10] -
			 m[4] * m[2] * m[11] +
			 m[4] * m[3] * m[10] +
			 m[8] * m[2] * m[7] -
			 m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] +
			  m[0] * m[7] * m[9] +
			  m[4] * m[1] * m[11] -
			  m[4] * m[3] * m[9] -
			  m[8] * m[1] * m[7] +
			  m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] -
			  m[0] * m[6] * m[9] -
			  m[4] * m[1] * m[10] +
			  m[4] * m[2] * m[9] +
			  m[8] * m[1] * m[6] -
			  m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

	if (det == 0)
		return false;

	det = 1.0 / det;

	int iterator = 0;
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			output->m[x][y] = inv[iterator] * det;
			iterator++;
		}
	}
	return true;
}

#if defined(__cplusplus)
}
#endif
