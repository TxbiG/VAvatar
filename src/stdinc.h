#pragma once


#ifdef __cplusplus
extern "C" {
#endif

#define bool char
#define true 1
#define false 0

#define PI          3.141592653589793238462643383279502884F
#define TAU         PI * 2
#define FLT_EPSILON 1.1920928955078125e-07F
#define INF         ((float)(1e+300 * 1e+300))
#define NAN         (-(float)(INFINITY * 0.0F))

#define RAD(x) ((x) * (PI / 180.0))
#define FACT(n) ((n) <= 1 ? 1 : (n) * FACT((n) - 1))
#define SIN(x) ( (x) - ((x)*(x)*(x))/FACT(3) + ((x)*(x)*(x)*(x)*(x))/FACT(5) - ((x)*(x)*(x)*(x)*(x)*(x)*(x))/FACT(7) )
#define COS(x) ( 1 - ((x)*(x))/FACT(2) + ((x)*(x)*(x)*(x))/FACT(4) - ((x)*(x)*(x)*(x)*(x)*(x))/FACT(6) )
#define TAN(x) (SIN(x) / COS(x))

#define clamp(x, a, b) (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))
#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))

#define arraysize(array) (sizeof(array)/sizeof(array[0]))

typedef signed char         int8;
typedef signed short        int16;
typedef signed int          int32;
typedef signed long long    int64;

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long long  uint64;

#ifdef _WIN64       // Windows (64-bit)
typedef uint64 size_t;
#elif _WIN32        // Windows (32-bit)
typedef uint32 size_t;
#elif __APPLE__     // macOS
#include <stddef.h>
typedef size_t size_t;
#elif __linux__     // Linux
#include <stddef.h>
typedef size_t size_t;
#endif

// Signed
#define MAX_INT8    ((int8)(0x7F))
#define MAX_INT16   ((int16)(0x7FFF))
#define MAX_INT32   ((int32)(0x7FFFFFFF))
#define MAX_INT64   ((int64)(0x7FFFFFFFFFFFFFFF))
#define MIN_INT8    ((int8)(~0x7F))
#define MIN_INT16   ((int16)~0x7FFF)
#define MIN_INT32   ((int32)(~0x7FFFFFFF))
#define MIN_INT64   ((int64)(~0x7FFFFFFFFFFFFFFF))

// Unsigned
#define MAX_UINT8   ((uint8)(0xFF))
#define MAX_UINT16  ((uint16)(0xFFFF))
#define MAX_UINT32  ((uint32)(0xFFFFFFFFu))
#define MAX_UINT64  ((uint64)(0xFFFFFFFFFFFFFFFF))
#define MIN_UINT8   ((uint8)0x00)
#define MIN_UINT16  ((uint16)0x0000)
#define MIN_UINT32  ((uint32)0x00000000)
#define MIN_UINT64  ((uint64)(0x0000000000000000))


typedef int64 Moss_Time;

// Memory management
void* Moss_malloc(size_t size);
void* Moss_calloc(size_t nmemb, size_t size);
void* Moss_realloc(void *mem, size_t size);
void* Moss_aligned_alloc(size_t alignment, size_t size);
void  Moss_aligned_free(void* mem);
void  Moss_free(void* mem);

// Random functions
int32  Moss_rand(int32 n);
float  Moss_randf(void);
int    Moss_randi(void);
void   Moss_srand(int64 seed);
int32  Moss_randi_range(int32 from, int32 to);
float  Moss_randf_range(float from, float to);
float  Moss_rand_gaussian(float mean, float stddev);
float  Moss_rand_unit();

// Math functions
float  Moss_abs(float x);
float  Moss_acos(float x);
float  Moss_asin(float x);
float  Moss_atan(float x);
float  Moss_atan2(float y, float x);
float  Moss_ceil(float x);
float  Moss_cos(float x);
float  Moss_exp(float x);
float  Moss_fabs(float x);
float  Moss_floor(float x);
float  Moss_fmod(float x, float y);
float  Moss_log(float x);
float  Moss_log10(float x);
float  Moss_pow(float base, float exp);
float  Moss_round(float x);
float  Moss_sin(float x);
float  Moss_sqrt(float x);
float  Moss_tan(float x);
float  Moss_trunc(float x);
float  Moss_cosh(float x);
float  Moss_sinh(float x);
float  Moss_tanh(float x);
float  Moss_acosh(float x);
float  Moss_asinh(float x);
float  Moss_atanh(float x);
float  Moss_hypot(float x, float y);
float  Moss_log2(float x);
float  Moss_exp2(float x);
float  Moss_lerp(float a, float b, float t);
float  Moss_smoothstep(float edge0, float edge1, float x);
float  Moss_deg_to_rad(float degrees);
float  Moss_rad_to_deg(float radians);
float  Moss_fast_inv_sqrt(float x);

// String functions
int    Moss_strlen(const char* str);
char*  Moss_strcpy(char* dest, const char* src);
char*  Moss_strncpy(char* dest, const char* src, size_t n);
int    Moss_strcmp(const char* s1, const char* s2);
int    Moss_strncmp(const char* s1, const char* s2, size_t n);
char*  Moss_strdup(const char* s);
char*  Moss_strchr(const char* s, int c);
char*  Moss_strrchr(const char* s, int c);
char*  Moss_strstr(const char* haystack, const char* needle);


#ifdef __cplusplus
}
#endif