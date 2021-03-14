#ifndef MATH_H
#define MATH_H

#define GRAVITATIONAL_CONSTANT 9.08

#define PI 3.1416

typedef struct Vector2 {
	float x;
	float y;
} Vector2_t;

Vector2_t vector2Difference(
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

float calculateGravity(
	const float gravitationalConstant,
	int mass1,
	int mass2,
	float distance
);

#endif
