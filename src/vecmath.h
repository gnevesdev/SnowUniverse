#ifndef VECMATH_H
#define VECMATH_H

#define GRAVITATIONAL_CONSTANT 0.6

#define PI 3.1416

typedef struct
{
	float x;
	float y;
} vector2_t;

vector2_t vector2_sub(vector2_t vec1, vector2_t vec2);
vector2_t vector2_sum(vector2_t vec1, vector2_t vec2);
vector2_t vector2_mul(vector2_t vec1, vector2_t vec2);
float vector2_distance(vector2_t vec1, vector2_t vec2);
vector2_t vector2_normalized(vector2_t vector);

#endif
