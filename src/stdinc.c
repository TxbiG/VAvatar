#include <Moss/stdinc.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <stddef.h>
#include <stdint.h>

// Memory management functions
void* Moss_malloc(size_t size) {
    return malloc(size);
}

void* Moss_calloc(size_t nmemb, size_t size) {
    return calloc(nmemb, size);
}

void* Moss_realloc(void *mem, size_t size) {
    return realloc(mem, size);
}

void* Moss_aligned_alloc(size_t alignment, size_t size) {
    void* ptr = NULL;
    posix_memalign(&ptr, alignment, size);
    return ptr;
}

void Moss_aligned_free(void* mem) {
    free(mem);
}

void Moss_free(void* mem) {
    free(mem);
}

// Random functions
int32 Moss_rand(int32 n) {
    return rand() % n;
}

float Moss_randf(void) {
    return (float)rand() / (float)RAND_MAX;
}

int Moss_randi(void) {
    return rand();
}

void Moss_srand(int64 seed) {
    srand((unsigned int)seed);
}

int32 Moss_randi_range(int32 from, int32 to) {
    return from + Moss_rand(to - from + 1);
}

float Moss_randf_range(float from, float to) {
    return from + (to - from) * Moss_randf();
}

float Moss_abs(float x) {
    return fabsf(x);
}

float Moss_acos(float x) {
    return acosf(x);
}

float Moss_asin(float x) {
    return asinf(x);
}

float Moss_atan(float x) {
    return atanf(x);
}

float Moss_atan2(float y, float x) {
    return atan2f(y, x);
}

float Moss_ceil(float x) {
    return ceilf(x);
}

float Moss_cos(float x) {
    return cosf(x);
}

float Moss_exp(float x) {
    return expf(x);
}

float Moss_fabs(float x) {
    return fabsf(x);
}

float Moss_floor(float x) {
    return floorf(x);
}

float Moss_fmod(float x, float y) {
    return fmodf(x, y);
}

float Moss_log(float x) {
    return logf(x);
}

float Moss_log10(float x) {
    return log10f(x);
}

float Moss_pow(float base, float exp) {
    return powf(base, exp);
}

float Moss_round(float x) {
    return roundf(x);
}

float Moss_sin(float x) {
    return sinf(x);
}

float Moss_sqrt(float x) {
    return sqrtf(x);
}

float Moss_tan(float x) {
    return tanf(x);
}

float Moss_trunc(float x) {
    return truncf(x);
}

float Moss_lerp(float a, float b, float t) {
    return a + t * (b - a);
}

float Moss_smoothstep(float edge0, float edge1, float x) {
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

float Moss_deg_to_rad(float degrees) {
    return degrees * (PI / 180.0f);
}

float Moss_rad_to_deg(float radians) {
    return radians * (180.0f / PI);
}

float Moss_fast_inv_sqrt(float x) {
    float xhalf = 0.5f * x;
    int i = *(int*)&x;
    i = 0x5f3759df - (i >> 1);
    x = *(float*)&i;
    x = x * (1.5f - xhalf * x * x);
    return x;
}
