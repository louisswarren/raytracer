#include <math.h>

#include "vector.h"

Vector
vectorsub(Vector a, Vector b)
{
	Vector c = {a.x - b.x, a.y - b.y, a.z - b.z};
	return c;
}

Vector
vectorscale(Vector a, double s)
{
	Vector b = {a.x * s, a.y * s, a.z * s};
	return b;
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
	return vectorscale(a, 1/scale);
}

Vector
cross(Vector a, Vector b)
{
	Vector r = {a.y * b.z - a.z * b.y,
	           -a.x * b.z + a.z * b.x,
	            a.x * b.y - a.y * b.x};
	return r;
}
