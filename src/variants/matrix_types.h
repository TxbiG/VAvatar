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

#include "vec_types.h"
#include "../stdinc.h"

template<int C, int R, typename T> struct mat;

// Float
typedef mat<4, 4, float>    mat4;
typedef mat<4, 4, float>    mat4x4;

// Integer
typedef mat<4, 4, int>    mat4i;
typedef mat<4, 4, int>    mat4x4i;


// Double
typedef mat<4, 4, double>    mat4d;
typedef mat<4, 4, double>    mat4x4d;


// unsigned 8 bit
typedef mat<4, 4, unsigned char>    mat4u8;
typedef mat<4, 4, unsigned char>    mat4x4u8;

// unsigned 16 bit
typedef mat<4, 4, unsigned short>    mat4u16;
typedef mat<4, 4, unsigned short>    mat4x4u16;

// unsigned 32 bit
typedef mat<4, 4, unsigned int>    mat4u32;
typedef mat<4, 4, unsigned int>    mat4x4u32;
// unsigned 64 bit
typedef mat<4, 4, unsigned long long>    mat4u64;
typedef mat<4, 4, unsigned long long>    mat4x4u64;


// signed 8 bit
typedef mat<4, 4, signed char>    mat4i8;
typedef mat<4, 4, signed char>    mat4x4i8;

// signed 16 bit
typedef mat<4, 4, signed short>    mat4i16;
typedef mat<4, 4, signed short>    mat4x4i16;
// signed 32 bit
typedef mat<4, 4, signed int>    mat4i32;
typedef mat<4, 4, signed int>    mat4x4i32;

// signed 64 bit
typedef mat<4, 4, signed long long>    mat4i64;
typedef mat<4, 4, signed long long>    mat4x4i64;

template<typename T>
struct mat<4, 4, T>
{
    	typedef vec<4, T> col_type;
		typedef vec<4, T> row_type;
		typedef mat<4, 4, T> type;
		typedef mat<4, 4, T> transpose_type;
		typedef T value_type;

	private:
		col_type value[4];

public:
    typedef size_t length_type;
    static constexpr length_type length() { return 4; }

    col_type& operator[](length_type i) { return value[i]; }
    constexpr const col_type& operator[](length_type i) const { return value[i]; }

    // Default constructor (Identity matrix)
    constexpr mat() 
        : value{ col_type(1, 0, 0, 0), col_type(0, 1, 0, 0), col_type(0, 0, 1, 0), col_type(0, 0, 0, 1) } {}

    // Scalar constructor (Diagonal matrix)
    explicit constexpr mat(T scalar) 
        : value{ col_type(scalar, 0, 0, 0), col_type(0, scalar, 0, 0), col_type(0, 0, scalar, 0), col_type(0, 0, 0, scalar) } {}

    // Column constructor
    constexpr mat(col_type const& v0, col_type const& v1, col_type const& v2, col_type const& v3)
        : value{ v0, v1, v2, v3 } {}

    // Matrix addition
    template<typename U>
    mat<4, 4, T>& operator+=(U s) {
        for (int i = 0; i < 4; ++i) value[i] += s;
        return *this;
    }

    template<typename U>
    mat<4, 4, T>& operator+=(mat<4, 4, U> const& m) {
        for (int i = 0; i < 4; ++i) value[i] += m[i];
        return *this;
    }

    // Matrix subtraction
    template<typename U>
    mat<4, 4, T>& operator-=(U s) {
        for (int i = 0; i < 4; ++i) value[i] -= s;
        return *this;
    }

    template<typename U>
    mat<4, 4, T>& operator-=(mat<4, 4, U> const& m) {
        for (int i = 0; i < 4; ++i) value[i] -= m[i];
        return *this;
    }

    // Matrix scalar multiplication
    template<typename U>
    mat<4, 4, T>& operator*=(U s) {
        for (int i = 0; i < 4; ++i) value[i] *= s;
        return *this;
    }

    // Matrix multiplication
    template<typename U>
    mat<4, 4, T>& operator*=(mat<4, 4, U> const& m) {
        mat<4, 4, T> result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result[i][j] = value[0][j] * m[i][0] + value[1][j] * m[i][1] + value[2][j] * m[i][2] + value[3][j] * m[i][3];
            }
        }
        *this = result;
        return *this;
    }

    // Matrix scalar division
    template<typename U>
    mat<4, 4, T>& operator/=(U s) {
        for (int i = 0; i < 4; ++i) value[i] /= s;
        return *this;
    }
};

// Translate
template<typename T>
inline mat<4, 4, T> translate(mat<4, 4, T> mat, vec<3, T> translate) {
    mat<4, 4, T> result = mat;
    result[3][0] += translate.x;
    result[3][1] += translate.y;
    result[3][2] += translate.z;
    return result;
}

// Rotate
template<typename T>
inline mat<4, 4, T> rotate(mat<4, 4, T> mat, vec<3, T> axis) {
    T sinX = SIN(axis.x), cosX = COS(axis.x);
    T sinY = SIN(axis.y), cosY = COS(axis.y);
    T sinZ = SIN(axis.z), cosZ = COS(axis.z);

    mat<4, 4, T> rotX = { {1, 0, 0, 0}, {0, cosX, -sinX, 0}, {0, sinX, cosX, 0}, {0, 0, 0, 1} };
    mat<4, 4, T> rotY = { {cosY, 0, sinY, 0}, {0, 1, 0, 0}, {-sinY, 0, cosY, 0}, {0, 0, 0, 1} };
    mat<4, 4, T> rotZ = { {cosZ, -sinZ, 0, 0}, {sinZ, cosZ, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1} };

    return mat * rotZ * rotY * rotX;
}

// Scale
template<typename T>
inline mat<4, 4, T> scale(mat<4, 4, T> mat, vec<3, T> scale) {
    mat<4, 4, T> result = mat;
    result[0][0] *= scale.x;
    result[1][1] *= scale.y;
    result[2][2] *= scale.z;
    return result;
}

// LookAt
template<typename T>
mat<4, 4, T> lookAtLH(vec<3, T> eye, vec<3, T> target, vec<3, T> up) {
    vec<3, T> forward = normalize(target - eye);
    vec<3, T> right = normalize(cross(up, forward));
    vec<3, T> adjustedUp = cross(forward, right);

    mat<4, 4, T> result;
    result(0, 0) = right.x;
    result(0, 1) = right.y;
    result(0, 2) = right.z;
    result(1, 0) = adjustedUp.x;
    result(1, 1) = adjustedUp.y;
    result(1, 2) = adjustedUp.z;
    result(2, 0) = forward.x;
    result(2, 1) = forward.y;
    result(2, 2) = forward.z;
    result(3, 0) = -dot(right, eye);
    result(3, 1) = -dot(adjustedUp, eye);
    result(3, 2) = -dot(forward, eye);
    result(3, 3) = 1;
    return result;
}

template<typename T>
inline mat<4, 4, T> lookAtRH(vec<3, T> eye, vec<3, T> target, vec<3, T> up) {
    vec<3, T> forward = normalize(eye - target); // Forward points away from the target
    vec<3, T> right = normalize(cross(up, forward));  // Right-handed coordinate system
    vec<3, T> adjustedUp = cross(forward, right);     // Adjusted up vector
    mat<4, 4, T> result;
    result(0, 0) = right.x;
    result(0, 1) = right.y;
    result(0, 2) = right.z;
    result(1, 0) = adjustedUp.x;
    result(1, 1) = adjustedUp.y;
    result(1, 2) = adjustedUp.z;
    result(2, 0) = forward.x;
    result(2, 1) = forward.y;
    result(2, 2) = forward.z;
    result(3, 0) = -dot(right, eye);
    result(3, 1) = -dot(adjustedUp, eye);
    result(3, 2) = -dot(forward, eye);
    result(3, 3) = 1;
    return result;
}


// Left-Handed
template<typename T>
inline mat<4, 4, T> orthoLH(T left, T right, T bottom, T top, T zNear, T zFar) {
    return { {2 / (right - left), 0, 0, 0}, {0, 2 / (top - bottom), 0, 0}, {0, 0, 1 / (zFar - zNear), 0}, {-(right + left) / (right - left), -(top + bottom) / (top - bottom), -zNear / (zFar - zNear), 1} };
}

// Right-Handed
template<typename T>
inline mat<4, 4, T> orthoRH(T left, T right, T bottom, T top, T zNear, T zFar) {
    return { {2 / (right - left), 0, 0, 0}, {0, 2 / (top - bottom), 0, 0}, {0, 0, -2 / (zFar - zNear), 0}, {-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(zFar + zNear) / (zFar - zNear), 1} };
}

// Left-Handed
template<typename T>
inline mat<4, 4, T> perspectiveLH(T fov, T aspect, T zNear, T zFar) {
    T tanHalfFov = TAN(fov / 2);
    return { {1 / (aspect * tanHalfFov), 0, 0, 0}, {0, 1 / tanHalfFov, 0, 0}, {0, 0, zFar / (zFar - zNear), 1}, {0, 0, -(zNear * zFar) / (zFar - zNear), 0} };
}

// Right-Handed
template<typename T>
inline mat<4, 4, T> perspectiveRH(T fov, T aspect, T zNear, T zFar) {
    T tanHalfFov = TAN(fov / 2);
    return { {1 / (aspect * tanHalfFov), 0, 0, 0}, {0, 1 / tanHalfFov, 0, 0}, {0, 0, -(zFar + zNear) / (zFar - zNear), -1}, {0, 0, -(2 * zNear * zFar) / (zFar - zNear), 0} };
}

// Generic LookAt
template<typename T>
inline mat<4, 4, T> lookAt(vec<3, T> eye, vec<3, T> center, vec<3, T> up) {
#	ifdef Include_Vulkan || Include_Metal || Include_DirectX
    return lookAtLH(eye, center, up); // Vulkan uses LH
#	else
    return lookAtRH(eye, center, up); // OpenGL uses RH
#	endif
}

// Generic Perspective
template<typename T>
inline mat<4, 4, T> perspective(T fov, T aspect, T zNear, T zFar) {
#   ifdef Include_Vulkan || Include_Metal || Include_DirectX
    return perspectiveLH(fov, aspect, zNear, zFar);
#   else
    return perspectiveRH(fov, aspect, zNear, zFar);
#   endif
}

// Generic Orthographic
template<typename T>
inline mat<4, 4, T> ortho(T left, T right, T bottom, T top, T zNear, T zFar) {
#   ifdef Include_Vulkan || Include_Metal || Include_DirectX
    return orthoLH(left, right, bottom, top, zNear, zFar);
#   else
    return orthoRH(left, right, bottom, top, zNear, zFar);
#   endif
}