#include <math.h>

#include "vector.h"

Vector vectorsub(Vector a, Vector b)
{
	return (Vector){a.x - b.x, a.y - b.y, a.z - b.z};
}

Vector vectorscale(Vector a, double s)
{
	return (Vector){a.x * s, a.y * s, a.z * s};
}

double dot(Vector a, Vector b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector normalise(Vector a)
{
	return vectorscale(a, 1/sqrt(dot(a, a)));
}

Vector cross(Vector a, Vector b)
{
	return (Vector){a.y * b.z - a.z * b.y,
	               -a.x * b.z + a.z * b.x,
	                a.x * b.y - a.y * b.x};
}
