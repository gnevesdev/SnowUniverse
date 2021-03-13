#ifndef MATH_H
#define MATH_H

typedef struct Vector2 {
	short int x;
	short int y;
} Vector2_t;

Vector2_t vector2Difference(
	Vector2_t vec1,
	Vector2_t vec2
);

#endif