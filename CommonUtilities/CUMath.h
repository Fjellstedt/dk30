/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */

   // NOTE(pf): Matrix is left handed.

#ifndef _CU_MATH_H_
#define _CU_MATH_H_

#pragma warning(disable:4201)
#include <cmath>

namespace Cryptic
{
	namespace Math
	{
		const F32 RadPi4 = cos(PI / 4);

		template<typename T = F32>
		union V2
		{
			struct
			{
				T x, y;
			};
			struct
			{
				T u, v;
			};
			T e[2];

			V2() { x = 0; y = 0; }
			V2(T x, T y) { this->x = x; this->y = y; }

			inline void Normalize()
			{
				T length = Length();

				x = (x / length);
				y = (y / length);
			}

			inline V2 Normalized()
			{
				T length = Length();
				return V2{(x / length), (y / length)};
			}

			inline T LengthSq()
			{
				return (x * x + y * y);
			}

			inline T Length()
			{
				return sqrt(x * x + y * y);
			}

			inline static V2<T> Hadamard(const V2<T> &v0, const V2<T> &v1)
			{
				V2<T> result;
				result.x = v0.x * v1.x;
				result.y = v0.y * v1.y;
				return result;
			}

			inline static F32 Inner(const V2<T> &v0, const V2<T> &v1)
			{
				F32 result = (v0.x * v1.x) + (v0.y * v1.y);
				return result;
			}

			inline V2 operator+(const V2 &other)
			{
				V2 result = {};
				result.x = x + other.x;
				result.y = y + other.y;
				return result;
			}

			inline void operator+=(const V2 &other)
			{
				x += other.x;
				y += other.y;
			}

			inline V2 operator-(const V2 &other)
			{
				V2 result = {};
				result.x = x - other.x;
				result.y = y - other.y;
				return result;
			}

			inline void operator-=(const V2 &other)
			{
				x -= other.x;
				y -= other.y;
			}

			inline void operator*=(const T value)
			{
				x *= value;
				y *= value;
			}

			inline V2 operator*(const T value)const
			{
				V2 result = {x * value, y * value};
				return result;
			}

			inline V2 GetDirectionFromRotation(T rotation)
			{
				V2 result{cos(rotation), -sin(rotation)};
				return result;
			}

			inline static F32 GetDistanceBetween(V2 v0, V2 v1)
			{
				V2 result = v1 - v0;
				return result.Length();
			}
		};

		typedef V2<F32> V2f;
		typedef V2<I32> V2i;

		namespace Direction2D
		{
			const V2f North = {0, 1};
			const V2f South = {0, -1};
			const V2f West = {-1, 0};
			const V2f East = {1, 0};
		}

		template<typename T>
		union V3
		{
			struct
			{
				T x, y, z;
			};
			struct
			{
				T r, g, b;
			};
			T e[3];

			V3() { x = 0; y = 0; z = 0; }
			V3(T x, T y, T z) { this->x = x; this->y = y; this->z = z; }

			inline void Normalize()
			{
				T oneOverLength = 1.f / Length();

				x = (x * oneOverLength);
				y = (y * oneOverLength);
				z = (z * oneOverLength);
			}

			inline V3 Normalized() const
			{
				T oneOverLength = 1 / Length();
				return V3{(x * oneOverLength), (y * oneOverLength), (z * oneOverLength)};
			}

			inline static V3<T> Normalized(const V3<T> &target)
			{
				V3<T> result = target.Normalized();
				return result;
			}

			inline T LengthSq()const
			{
				return (x * x + y * y + z * z);
			}

			inline static T LengthSq(const V3<T> &target)
			{
				return target.LengthSq();
			}

			inline T Length() const
			{
				Assert(!(x == 0 && y == 0 && z == 0));
				return sqrt(x * x + y * y + z * z);
			}

			inline static F32 Inner(const V3<T> &v0, const V3<T> &v1)
			{
				F32 result = (v0.x * v1.x) + (v0.y * v1.y) + (v0.z * v1.z);
				return result;
			}

			inline static V3<T> Cross(const V3<T> &v0, const V3<T> &v1)
			{
				V3<T> result = V3<T>((v0.y * v1.z) - (v0.z * v1.y),
					(v0.z * v1.x) - (v0.x * v1.z),
									 (v0.x * v1.y) - (v0.y * v1.x));
				return result;
			}

			inline V3 operator+(const V3 &other)
			{
				V3 result = {};
				result.x = x + other.x;
				result.y = y + other.y;
				result.z = z + other.z;
				return result;
			}

			inline void operator+=(const V3 &other)
			{
				x += other.x;
				y += other.y;
				z += other.z;
			}

			inline V3 operator-(const V3 &other)
			{
				V3 result = {};
				result.x = x - other.x;
				result.y = y - other.y;
				result.z = z - other.z;
				return result;
			}

			inline V3 operator-()
			{
				V3 result = {};
				result.x = -x;
				result.y = -y;
				result.z = -z;
				return result;
			}

			inline void operator-=(const V3 &other)
			{
				x -= other.x;
				y -= other.y;
				z -= other.z;
			}

			inline void operator*=(const T value)
			{
				x *= value;
				y *= value;
				z *= value;
			}

			inline V3 operator*(const T value)
			{
				V3 result = {x * value, y * value, z * value};
				return result;
			}
		};

		typedef V3<F32> V3f;
		typedef V3<I32> V3i;
		namespace Direction3D
		{
			const V3f Left = {-1, 0, 0};
			const V3f Right = {1, 0, 0};
			const V3f Down = {0, -1, 0};
			const V3f Up = {0, 1, 0};
			const V3f Back = {0, 0, -1};
			const V3f Forward = {0, 0, 1};
		}

		template<typename T>
		union V4
		{
			V4() { x = 0; y = 0; z = 0; w = 0; }
			V4(T x, T y, T z, T w) { this->x = x; this->y = y; this->z = z, this->w = w; }

			struct
			{
				T x, y, z, w;
			};
			struct
			{
				T r, g, b, a;
			};
			T e[4];

			inline void Normalize()
			{
				T oneOverLength = Length();

				x = (x / oneOverLength);
				y = (y / oneOverLength);
				z = (z / oneOverLength);
				w = (w / oneOverLength);
			}

			inline V4 Normalized()
			{
				T oneOverLength = 1 / Length();
				return V4{(x * oneOverLength), (y * oneOverLength), (z * oneOverLength), (w * oneOverLength)};
			}

			inline T LengthSq()
			{
				return (x * x + y * y + z * z + w * w);
			}

			inline T Length()
			{
				Assert(!(x == 0 && y == 0));
				return sqrt(x * x + y * y + z * z + w * w);
			}

			inline V4 operator+(const V4 &other)
			{
				V4 result = {};
				result.x = x + other.x;
				result.y = y + other.y;
				result.z = z + other.z;
				result.w = w + other.w;
				return result;
			}

			inline void operator+=(const V4 &other)
			{
				x += other.x;
				y += other.y;
				z += other.z;
				w += other.w;
			}

			inline V4 operator-(const V4 &other)
			{
				V4 result = {};
				result.x = x - other.x;
				result.y = y - other.y;
				result.z = z - other.z;
				result.w = w - other.w;
				return result;
			}

			inline void operator-=(const V4 &other)
			{
				x -= other.x;
				y -= other.y;
				z -= other.z;
				w -= other.w;
			}

			inline void operator*=(const T value)
			{
				x *= value;
				y *= value;
				z *= value;
				w *= value;
			}

			inline V4 operator*(const T value) const
			{
				V4 result = {x * value, y * value, z * value, w * value};
				return result;
			}
		};

		typedef V4<F32> V4f;
		typedef V4<I32> V4i;

		template<typename T>
		struct Rect2
		{
			union
			{
				V2<T> min;
				struct
				{
					T left;
					T top;
				};
			};

			union
			{
				V2<T> max;
				struct
				{
					T right;
					T bottom;
				};
			};

			Rect2() {}

			Rect2(V2<T> center, V2<T> halfDim)
			{
				min = center - halfDim;
				max = center + halfDim;
			}

			inline B32 Contains(V2<T> pos)
			{
				B32 result = false;
				if (pos.x >= min.x &&
					pos.y >= min.y &&
					pos.x < max.x &&
					pos.y < max.y)
				{
					result = true;
				}
				return result;
			}

			inline void Expand(V2<T> size)
			{
				min -= size;
				max += size;
			}

			inline void operator*=(const T value)
			{
				left *= value;
				top *= value;
				bottom *= value;
				right *= value;
			}

		};

		typedef Rect2<F32> Rect2f;
		typedef Rect2<I32> Rect2i;

		inline F32 SquareF32(F32 value)
		{
			F32 result = value * value;
			return result;
		}

#define HALF_RAND_MAX (RAND_MAX/2)
		inline F32 NormalizedRand01()
		{
			return (static_cast<F32>(rand()) / RAND_MAX);
		}

		inline F32 NormalizedRandNeg11()
		{
			return (static_cast<F32>(rand() - HALF_RAND_MAX) / HALF_RAND_MAX);
		}

		union M4x4
		{
			struct
			{
				F32 _11, _12, _13, _14;
				F32 _21, _22, _23, _24;
				F32 _31, _32, _33, _34;
				F32 _41, _42, _43, _44;
			};
			F32 e[16];

			inline V3f GetTranslation()
			{
				V3f result;
				result.x = _41;
				result.y = _42;
				result.z = _43;
				return result;
			}

			inline void SetTranslation(const V3f &translation)
			{
				_41 = translation.x;
				_42 = translation.y;
				_43 = translation.z;
			}

			inline void AddTranslation(const V3f &translation)
			{
				this->_41 += translation.x;
				this->_42 += translation.y;
				this->_43 += translation.z;
			}

			inline void AddTranslation(const V4f &translation)
			{
				this->_41 += translation.x;
				this->_42 += translation.y;
				this->_43 += translation.z;
				this->_44 += translation.w;
			}

			inline void ApplyRotation(const M4x4 &other)
			{
				// NOTE(pf): Based of DX11 Math Library, caching invariants.
				F32 x = _11;
				F32 y = _12;
				F32 z = _13;
				F32 w = _14;
				// .. first row.
				_11 = (other._11 * x) + (other._21 * y) + (other._31 * z) + (other._41 * w);
				_12 = (other._12 * x) + (other._22 * y) + (other._32 * z) + (other._42 * w);
				_13 = (other._13 * x) + (other._23 * y) + (other._33 * z) + (other._43 * w);
				_14 = (other._14 * x) + (other._24 * y) + (other._34 * z) + (other._44 * w);

				// .. second row .. etc
				x = _21;
				y = _22;
				z = _23;
				w = _24;
				_21 = (other._11 * x) + (other._21 * y) + (other._31 * z) + (other._41 * w);
				_22 = (other._12 * x) + (other._22 * y) + (other._32 * z) + (other._42 * w);
				_23 = (other._13 * x) + (other._23 * y) + (other._33 * z) + (other._43 * w);
				_24 = (other._14 * x) + (other._24 * y) + (other._34 * z) + (other._44 * w);

				x = _31;
				y = _32;
				z = _33;
				w = _34;
				_31 = (other._11 * x) + (other._21 * y) + (other._31 * z) + (other._41 * w);
				_32 = (other._12 * x) + (other._22 * y) + (other._32 * z) + (other._42 * w);
				_33 = (other._13 * x) + (other._23 * y) + (other._33 * z) + (other._43 * w);
				_34 = (other._14 * x) + (other._24 * y) + (other._34 * z) + (other._44 * w);
			}

			inline static M4x4 CreateRotationX(F32 rad)
			{
				F32 cosValue = cos(rad);
				F32 sinValue = sin(rad);

				M4x4 result;
				result._11 = 1;
				result._12 = 0;
				result._13 = 0;
				result._14 = 0;

				result._21 = 0;
				result._22 = cosValue;
				result._23 = sinValue;
				result._24 = 0;

				result._31 = 0;
				result._32 = -sinValue;
				result._33 = cosValue;
				result._34 = 0;

				result._41 = 0;
				result._42 = 0;
				result._43 = 0;
				result._44 = 1;
				return result;
			}

			inline static M4x4 CreateRotationY(F32 rad)
			{
				F32 cosValue = cos(rad);
				F32 sinValue = sin(rad);

				M4x4 result;
				result._11 = cosValue;
				result._12 = 0;
				result._13 = -sinValue;
				result._14 = 0;

				result._21 = 0;
				result._22 = 1;
				result._23 = 0;
				result._24 = 0;

				result._31 = sinValue;
				result._32 = 0;
				result._33 = cosValue;
				result._34 = 0;

				result._41 = 0;
				result._42 = 0;
				result._43 = 0;
				result._44 = 1;
				return result;
			}

			inline static M4x4 CreateRotationZ(F32 rad)
			{
				F32 cosValue = cos(rad);
				F32 sinValue = sin(rad);

				M4x4 result;
				result._11 = cosValue;
				result._12 = sinValue;
				result._13 = 0;
				result._14 = 0;

				result._21 = -sinValue;
				result._22 = cosValue;
				result._23 = 0;
				result._24 = 0;

				result._31 = 0;
				result._32 = 0;
				result._33 = 1;
				result._34 = 0;

				result._41 = 0;
				result._42 = 0;
				result._43 = 0;
				result._44 = 1;
				return result;
			}

			inline static M4x4 Identity()
			{
				M4x4 result;
				result._11 = 1;
				result._12 = 0;
				result._13 = 0;
				result._14 = 0;

				result._21 = 0;
				result._22 = 1;
				result._23 = 0;
				result._24 = 0;

				result._31 = 0;
				result._32 = 0;
				result._33 = 1;
				result._34 = 0;

				result._41 = 0;
				result._42 = 0;
				result._43 = 0;
				result._44 = 1;

				return result;
			}
		};
	}
}
#endif //!_MATH_H_
