#ifndef VECMATH_H
#define VECMATH_H

#define GRAVITATIONAL_CONSTANT 0.6

#define PI 3.1416

typedef struct Vector2
{
	float x;
	float y;
} Vector2_t;

Vector2_t vector2Sub(
	Vector2_t vec1,
	Vector2_t vec2
);

Vector2_t vector2Sum(
	Vector2_t vec1,
	Vector2_t vec2
);

Vector2_t vector2Mul(
	Vector2_t vec1,
	Vector2_t vec2
);

float vector2Distance(
	Vector2_t vec1,
	Vector2_t vec2
);

Vector2_t vector2Normalized(
	Vector2_t vector
);

#endif
