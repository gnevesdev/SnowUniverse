#ifndef VECMATH_H
#define VECMATH_H

#define GRAVITATIONAL_CONSTANT 0.6

#define PI 3.1416

typedef struct
{
  float x;
  float y;
} Vector2_t;

Vector2_t vector2_sub       (Vector2_t vec1, Vector2_t vec2);
Vector2_t vector2_sum       (Vector2_t vec1, Vector2_t vec2);
Vector2_t vector2_mul       (Vector2_t vec1, Vector2_t vec2);
float     vector2_distance  (Vector2_t vec1, Vector2_t vec2);
Vector2_t vector2_normalized(Vector2_t vector);

#endif

