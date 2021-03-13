#include "vecmath.h"
#include <math.h>

static float squareRoot(float value) {
	if (value < 0)
		value = fabs(value);
	else if (value == 0)
		return 0;

	return (float)sqrt(value);
}

static float square(float value) {
	return value * value;
}

Vector2_t vector2Difference(Vector2_t vec1, Vector2_t vec2) {
	return (Vector2_t) {
		vec1.x - vec2.x,
		vec1.y - vec2.y
	};
}

/* unsigned */ float vector2Distance(Vector2_t vec1, Vector2_t vec2) {
	return squareRoot(square(vec1.x - vec2.x) + square(vec1.y - vec2.y));
}

Vector2_t vector2Normalized(Vector2_t vector) {
	float magnitude = vector2Distance(
		vector,
		(Vector2_t) {0, 0}
	);
	
	if (magnitude == 0)
		return (Vector2_t) {0, 0};

	vector.x /= magnitude;
	vector.y /= magnitude;

	return vector;
}
