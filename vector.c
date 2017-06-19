#include <math.h>

#include "vector.h"

Vector vecadd(Vector a, Vector b)
{
	return (Vector){a.x + b.x, a.y + b.y, a.z + b.z};
}

Vector vecsub(Vector a, Vector b)
{
	return (Vector){a.x - b.x, a.y - b.y, a.z - b.z};
}

Vector vecscale(Vector a, double s)
{
	return (Vector){a.x * s, a.y * s, a.z * s};
}

double vecdot(Vector a, Vector b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

double vecmagnitude(Vector a)
{
	return sqrt(vecdot(a, a));
}

Vector vecnormalise(Vector a)
{
	return vecscale(a, 1/vecmagnitude(a));
}

Vector veccross(Vector a, Vector b)
{
	return (Vector){a.y * b.z - a.z * b.y,
	               -a.x * b.z + a.z * b.x,
	                a.x * b.y - a.y * b.x};
}
