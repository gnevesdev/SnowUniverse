#include "math.h"

Vector2_t vector2Difference(Vector2_t vec1, Vector2_t vec2) {
	return (Vector2_t) {vec1.x - vec2.x, vec1.y - vec2.y};
}
