//                        MIT License
//
//                  Copyright (c) 2024 Toby
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#pragma once

template<int L, typename T> struct vec;

typedef vec<2, float>  vec2;
typedef vec<3, float>  vec3;
typedef vec<4, float>  vec4;

typedef vec<2, int>    vec2i;
typedef vec<3, int>    vec3i;
typedef vec<4, int>    vec4i;

typedef vec<2, double> vec2d;
typedef vec<3, double> vec3d;
typedef vec<4, double> vec4d;

typedef vec<2, unsigned char> vec2u8;
typedef vec<3, unsigned char> vec3u8;
typedef vec<4, unsigned char> vec4u8;

typedef vec<2, unsigned short> vec2u16;
typedef vec<3, unsigned short> vec3u16;
typedef vec<4, unsigned short> vec4u16;

typedef vec<2, unsigned int> vec2u32;
typedef vec<3, unsigned int> vec3u32;
typedef vec<4, unsigned int> vec4u32;

typedef vec<2, unsigned long long> vec2u64;
typedef vec<3, unsigned long long> vec3u64;
typedef vec<4, unsigned long long> vec4u64;

typedef vec<2, signed char> vec2i8;
typedef vec<3, signed char> vec3i8;
typedef vec<4, signed char> vec4i8;

typedef vec<2, signed short> vec2i16;
typedef vec<3, signed short> vec3i16;
typedef vec<4, signed short> vec4i16;

typedef vec<2, signed int> vec2i32;
typedef vec<3, signed int> vec3i32;
typedef vec<4, signed int> vec4i32;

typedef vec<2, unsigned long long> vec2i64;
typedef vec<3, unsigned long long> vec3i64;
typedef vec<4, unsigned long long> vec4i64;

typedef struct vec<4, float> Rect;
typedef struct vec<4, float> Color;
typedef struct vec<4, int> Recti;

template<typename T>
struct vec<2, T> 
{
    union {T x, y; };

    vec();
    vec(const T& x, const T& y) : x(x), y(y) {}

    // Operator Overloads
    vec<2, T> & operator+=(const vec<2, T> & b) { x += b.x; y += b.y; return *this; }
    vec<2, T> & operator-=(const vec<2, T> & b) { x -= b.x; y -= b.y; return *this; }
    vec<2, T> & operator*=(float b) { x *= b; y *= b; return *this; }
    vec<2, T>  operator-() const { return { -x, -y }; }

    // Static function to handle binary operations
    friend vec<2, T>  operator+(vec<2, T>  a, const vec<2, T> & b) { return { a.x + b.x, a.y + b.y }; }
    friend vec<2, T>  operator-(vec<2, T>  a, const vec<2, T> & b) { return { a.x - b.x, a.y - b.y }; }
    friend vec<2, T>  operator*(vec<2, T>  a, float b) {  return { a.x * b, a.y * b }; }
    friend bool operator==(const vec<2, T> & a, const vec<2, T> & b) { return a.x == b.x && a.y == b.y; }
    friend bool operator!=(const vec<2, T> & a, const vec<2, T> & b) { return a.x != b.x || a.y != b.y; }

    T magnitude() const { return sqrt(x * x + y * y); }
    vec<2, T> normalize() const { float mag = magnitude(); return (mag == 0.0f) ? Vec2(0, 0) : Vec2(x / mag, y / mag); }
};

template<typename T>
struct vec<3, T> 
{
    union {T x, y, z; };

    vec(const T& x, const T& y, const T& z) : x(x), y(y), z(z) {}

    // Operator Overloads
    vec<3, T>& operator+=(const vec<3, T>& b) { x += b.x; y += b.y; z += b.z; return *this; }
    vec<3, T>& operator-=(const vec<3, T>& b) { x -= b.x; y -= b.y; z -= b.z; return *this; }
    vec<3, T>& operator*=(float b) {  x *= b; y *= b; z *= b; return *this; }
    vec<3, T> operator-() const { return { -x, -y, -z }; }

    // Static function to handle binary operations
    friend vec<3, T> operator+(vec<3, T> a, const vec<3, T>& b) { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
    friend vec<3, T> operator-(vec<3, T> a, const vec<3, T>& b) { return { a.x - b.x, a.y - b.y, a.z - b.z }; }
    friend vec<3, T> operator*(vec<3, T> a, float b) { return { a.x * b, a.y * b, a.z * b }; }
    friend bool operator==(const vec<3, T>& a, const vec<3, T>& b) { return a.x == b.x && a.y == b.y && a.z == b.z; }
    friend bool operator!=(const vec<3, T>& a, const vec<3, T>& b) { return a.x != b.x || a.y != b.y || a.z != b.z; }

    T magnitude(const vec<3, T>& self) const { return sqrt(x * x + y * y + z * z); }
    vec<3, T> normalize() const { float mag = magnitude(); return (mag == 0.0f) ? Vec3(0, 0, 0) : Vec3(x / mag, y / mag, z / mag); }
};

template<typename T>
struct vec<4, T> 
{
    union { T x, y, z, w; };

    vec() : data{} {}
    vec(const T& x, const T& y, const T& z, const T& w) : x(x), y(y), z(z), w(w) {}

    vec<4, T>& operator+=(const vec<4, T> & b) { x += b.x; y += b.y; z += b.z; w += b.w;  return *this; }
    vec<4, T>& operator-=(const vec<4, T> & b) { x -= b.x; y -= b.y; z -= b.z; w -= b.w; return *this; }
    vec<4, T>& operator*=(float b) { x *= b; y *= b; z *= b; w *= b; return *this; }
    vec<4, T>  operator-() const {  return { -x, -y, -z, -w }; }

    // Static function to handle binary operations
    friend vec<4, T>  operator+(vec<4, T>  a, const vec<4, T> & b) { return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; }
    friend vec<4, T>  operator-(vec<4, T>  a, const vec<4, T> & b) {  return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; }
    friend vec<4, T>  operator*(vec<4, T>  a, float b) { return { a.x * b, a.y * b, a.z * b, a.w * b }; }
    friend bool operator==(const vec<4, T> & a, const vec<4, T> & b) { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }
    friend bool operator!=(const vec<4, T> & a, const vec<4, T> & b) { return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w; }

    T magnitude() const { return sqrt(x * x + y * y + z * z + w * w); }
    vec<4, T> normalize() const { float mag = magnitude(); return (mag == 0.0f) ? Vec4(0, 0, 0, 0) : Vec4(x / mag, y / mag, z / mag, w / mag); }
};

inline const float value_ptr(float value);
inline float radians(float degrees);

template<typename T>
inline float dot(vec<2, T> a, vec<2, T> b) { return a.x * b.x + a.y * b.y; }

template<typename T>
inline float dot(vec<3, T> a, vec<3, T> b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
template<typename T>
inline float dot(vec<4, T> a, vec<4, T> b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;}

template<typename T>
inline float cross(vec<2, T> a, vec<2, T> b) { return a.x * b.y - a.y * b.x; }
template<typename T>
inline vec3 cross(vec<3, T> a, vec<3, T> b) {return vec3( a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x );}
template<typename T>
inline vec4 cross(vec<4, T> a, vec<4, T> b) { vec3 c = cross(vec3(a.x, a.y, a.z), vec3(b.x, b.y, b.z)); return vec4(c.x, c.y, c.z, 0.0f);}

inline vec2 min(const vec2& lhs, const vec2& rhs)                    { return vec2(lhs.x < rhs.x ? lhs.x : rhs.x, lhs.y < rhs.y ? lhs.y : rhs.y); }
inline vec2 max(const vec2& lhs, const vec2& rhs)                    { return vec2(lhs.x >= rhs.x ? lhs.x : rhs.x, lhs.y >= rhs.y ? lhs.y : rhs.y); }
inline vec2 clamp(const vec2& v, const vec2& mn, const vec2& mx)     { return vec2((v.x < mn.x) ? mn.x : (v.x > mx.x) ? mx.x : v.x, (v.y < mn.y) ? mn.y : (v.y > mx.y) ? mx.y : v.y); }
inline vec3 min(const vec3& lhs, const vec3& rhs)                    { return vec3(lhs.x < rhs.x ? lhs.x : rhs.x, lhs.y < rhs.y ? lhs.y : rhs.y, lhs.z < rhs.z ? lhs.z : rhs.z); }
inline vec3 max(const vec3& lhs, const vec3& rhs)                    { return vec3(lhs.x >= rhs.x ? lhs.x : rhs.x, lhs.y >= rhs.y ? lhs.y : rhs.y, lhs.z >= rhs.z ? lhs.z : rhs.z); }
inline vec3 clamp(const vec3& v, const vec3& mn, const vec3& mx)     { return vec3((v.x < mn.x) ? mn.x : (v.x > mx.x) ? mx.x : v.x, (v.y < mn.y) ? mn.y : (v.y > mx.y) ? mx.y : v.y, (v.z < mn.z) ? mn.z : (v.z > mx.z) ? mx.z : v.z); }
inline vec4 min(const vec4& lhs, const vec4& rhs)                    {return vec4(lhs.x < rhs.x ? lhs.x : rhs.x, lhs.y < rhs.y ? lhs.y : rhs.y, lhs.z < rhs.z ? lhs.z : rhs.z, lhs.w < rhs.w ? lhs.w : rhs.w); }
inline vec4 max(const vec4& lhs, const vec4& rhs)                    { return vec4(lhs.x >= rhs.x ? lhs.x : rhs.x, lhs.y >= rhs.y ? lhs.y : rhs.y, lhs.z >= rhs.z ? lhs.z : rhs.z, lhs.w >= rhs.w ? lhs.w : rhs.w); }
inline vec4 clamp(const vec4& v, const vec4& mn, const vec4& mx)     { return vec4((v.x < mn.x) ? mn.x : (v.x > mx.x) ? mx.x : v.x, (v.y < mn.y) ? mn.y : (v.y > mx.y) ? mx.y : v.y, (v.z < mn.z) ? mn.z : (v.z > mx.z) ? mx.z : v.z,(v.w < mn.w) ? mn.w : (v.w > mx.w) ? mx.w : v.w); }

inline vec2 lerp(const vec2& a, const vec2& b, float t)        { return vec2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t); }
inline vec3 lerp(const vec3& a, const vec3& b, const vec3& t)  { return vec3(a.x + (b.x - a.x) * t.x, a.y + (b.y - a.y) * t.y, a.z + (b.z - a.z) * t.z); }
inline vec4 lerp(const vec4& a, const vec4& b, float t)        { return vec4(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t); }


