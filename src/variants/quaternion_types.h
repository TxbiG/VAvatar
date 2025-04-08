#pragma once

#include "../std/math.h"
#include "matrix_types.h"

template<typename T>
struct Quaternion
{
	// -- Implementation detail --

	typedef Quaternion<T> type;
	typedef T value_type;

#	ifdef MOSS_FORCE_QUAT_DATA_WXYZ
				T w, x, y, z;
#	else
				T x, y, z, w;
#	endif

	typedef length_t length_type;

	// Return the count of components of a quaternion
	static constexpr length_type length(){return 4;}

	constexpr T & operator[](length_type i);
	constexpr T const& operator[](length_type i) const;

	// -- Implicit basic constructors --

	constexpr Quaternion() GLM_DEFAULT;
	constexpr Quaternion(Quaternion<T, Q> const& q) GLM_DEFAULT;
	template<qualifier P>
	constexpr Quaternion(qua<T, P> const& q);

	// -- Explicit basic constructors --

	constexpr Quaternion(T s, vec<3, T, Q> const& v);
	constexpr Quaternion(T w, T x, T y, T z);

	// -- Conversion constructors --

	template<typename U, qualifier P>
	constexpr Quaternion(qua<U, P> const& q);

	// Create a quaternion from two normalized axis
	// @param u A first normalized axis
	// @param v A second normalized axis
	// @see gtc_quaternion
	// @see http://lolengine.net/blog/2013/09/18/beautiful-maths-quaternion-from-vectors
	Quaternion(vec<3, T, Q> const& u, vec<3, T, Q> const& v);

		/// Build a quaternion from euler angles (pitch, yaw, roll), in radians.
		constexpr qua(vec<3, T, Q> const& eulerAngles);
		Quaternion Quaternion(mat<3, 3, T, Q> const& q);
		Quaternion Quaternion(mat<4, 4, T, Q> const& q);

		// -- Unary arithmetic operators --

		GLM_FUNC_DECL constexpr qua<T, Q>& operator=(qua<T, Q> const& q) GLM_DEFAULT;

		template<typename U>
		 constexpr Quaternion<T, Q>& operator=(Quaternion<U, Q> const& q);
		template<typename U>
		constexpr Quaternion<T, Q>& operator+=(Quaternion<U, Q> const& q);
		template<typename U>
		constexpr Quaternion<T, Q>& operator-=(Quaternion<U, Q> const& q);
		template<typename U>
		constexpr Quaternion<T, Q>& operator*=(Quaternion<U, Q> const& q);
		template<typename U>
		constexpr Quaternion<T, Q>& operator*=(U s);
		template<typename U>
		constexpr Quaternion<T, Q>& operator/=(U s);
	};

	// -- Unary bit operators --

template<typename T>
constexpr Quaternion<T> operator+(Quaternion<T> const& q);

template<typename T>
constexpr Quaternion<T> operator-(Quaternion<T> const& q);

	// -- Binary operators --

template<typename T>
constexpr Quaternion<T> operator+(Quaternion<T> const& q, Quaternion<T> const& p);

template<typename T>
constexpr Quaternion<T> operator-(Quaternion<T> const& q, Quaternion<T> const& p);

template<typename T>
constexpr Quaternion<T> operator*(Quaternion<T> const& q, Quaternion<T> const& p);

template<typename T>
constexpr Vec<3, T> operator*(Quaternion<T> const& q, Vec<3, T> const& v);

template<typename T>
constexpr Vec<3, T> operator*(Vec<3, T> const& v, Quaternion<T> const& q);

template<typename T>
constexpr Vec<4, T> operator*(Quaternion<T> const& q, Vec<4, T> const& v);

template<typename T>
constexpr Vec<4, T> operator*(Vec<4, T> const& v, Quaternion<T> const& q);

template<typename T>
constexpr Quaternion<T> operator*(Quaternion<T> const& q, T const& s);

template<typename T>
constexpr Quaternion<T> operator*(T const& s, Quaternion<T> const& q);

template<typename T>
constexpr Quaternion<T> operator/(Quaternion<T> const& q, T const& s);

template<typename T>
constexpr bool operator==(Quaternion<T> const& q1, Quaternion<T> const& q2);

template<typename T>
constexpr bool operator!=(Quaternion<T> const& q1, Quaternion<T> const& q2);


inline Quaternion normalize(Quaternion const& q);

inline float length(Quaternion const& q);


template<typename T>
vec<3, T> eulerAngles(Quaternion<T> const& x);

	/// Returns roll value of euler angles expressed in radians.
	///
	/// @tparam T Floating-point scalar types.
	///
	/// @see gtc_quaternion
template<typename T>
T roll(Quaternion<T> const& x);

/// Returns pitch value of euler angles expressed in radians.
///
/// @tparam T Floating-point scalar types.
///
/// @see gtc_quaternion
template<typename T>
T pitch(Quaternion<T> const& x);

/// Returns yaw value of euler angles expressed in radians.
///
/// @tparam T Floating-point scalar types.
///
/// @see gtc_quaternion
template<typename T>
T yaw(Quaternion<T> const& x);

/// Converts a quaternion to a 3 * 3 matrix.
///
/// @tparam T Floating-point scalar types.
///
/// @see gtc_quaternion
template<typename T>
mat<3, 3, T> mat3_cast(Quaternion<T> const& x);

/// Converts a quaternion to a 4 * 4 matrix.
///
/// @tparam T Floating-point scalar types.
///
/// @see gtc_quaternion
template<typename T>
mat<4, 4, T> mat4_cast(qua<T, Q> const& x);

/// Converts a pure rotation 3 * 3 matrix to a quaternion.
///
/// @tparam T Floating-point scalar types.
///
/// @see gtc_quaternion
template<typename T>
Quaternion<T> quat_cast(mat<3, 3, T> const& x);

	/// Converts a pure rotation 4 * 4 matrix to a quaternion.
	///
	/// @tparam T Floating-point scalar types.
	///
	/// @see gtc_quaternion
template<typename T>
Quaternion<T> quat_cast(mat<4, 4, T, Q> const& x);

/// Returns the component-wise comparison result of x < y.
///
/// @tparam T Floating-point scalar types
/// @tparam Q Value from qualifier enum
///
/// @see ext_quaternion_relational
template<typename T>
Vec<4, bool> lessThan(Quaternion<T> const& x, Quaternion<T> const& y);

/// Returns the component-wise comparison of result x <= y.
///
/// @tparam T Floating-point scalar types
/// @tparam Q Value from qualifier enum
///
/// @see ext_quaternion_relational
template<typename T>
Vec<4, bool> lessThanEqual(Quaternion<T> const& x, Quaternion<T> const& y);

/// Returns the component-wise comparison of result x > y.
///
/// @tparam T Floating-point scalar types
/// @tparam Q Value from qualifier enum
///
/// @see ext_quaternion_relational
template<typename T>
Vec<4, bool> greaterThan(Quaternion<T> const& x, Quaternion<T> const& y);

	/// Returns the component-wise comparison of result x >= y.
	///
	/// @tparam T Floating-point scalar types
	/// @tparam Q Value from qualifier enum
	///
	/// @see ext_quaternion_relational
template<typename T>
Vec<4, bool> greaterThanEqual(Quaternion<T> const& x, Quaternion<T> const& y);


/// Build a right-handed look at quaternion.
///
/// @param direction Desired forward direction onto which the -z-axis gets mapped. Needs to be normalized.
/// @param up Up vector, how the camera is oriented. Typically (0, 1, 0).
template<typename T>
Quaternion<T> quatLookAtRH(vec<3, T> const& direction, vec<3, T> const& up);

/// Build a left-handed look at quaternion.
///
/// @param direction Desired forward direction onto which the +z-axis gets mapped. Needs to be normalized.
/// @param up Up vector, how the camera is oriented. Typically (0, 1, 0).
template<typename T>
Quaternion<T> quatLookAtLH(Vec<3, T> const& direction, Vec<3, T> const& up);

/// Build a look at quaternion based on the default handedness.
///
/// @param direction Desired forward direction. Needs to be normalized.
/// @param up Up vector, how the camera is oriented. Typically (0, 1, 0).
template<typename T>
Quaternion<T> quatLookAt(vec<3, T> const& direction, vec<3, T> const& up);
