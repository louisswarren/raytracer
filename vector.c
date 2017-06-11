#include <math.h>

#include "vector.h"

Vector
vectorsub(Vector a, Vector b)
{
	Vector c = {a.x - b.x, a.y - b.y, a.z - b.z};
	return c;
}

double
dot(Vector a, Vector b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector
normalise(Vector a)
{
	double scale = sqrt(dot(a, a));
	Vector n = {a.x / scale, a.y / scale, a.z / scale};
	return n;
}
