#ifndef TYPES_VECTOR_H_
#define TYPES_VECTOR_H_

namespace types {
	template<typename T>
	struct Vector2 {
		Vector2() = default;

		constexpr Vector2(T first, T second)
		: x(first)
		, y(second)
		{

		}

		union {
			T x;
			T width;
		};

		union {
			T y;
			T height;
		};
	};

	using Vector2i = Vector2<int>;
}

#endif // TYPES_VECTOR_H_
