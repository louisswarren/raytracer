#include <math.h>

#include "point.h"

Point
pointsub(Point a, Point b)
{
	Point c = {a.x - b.x, a.y - b.y, a.z - b.z};
	return c;
}

double
dot(Point a, Point b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Point
normalise(Point a)
{
	double scale = sqrt(dot(a, a));
	Point n = {a.x / scale, a.y / scale, a.z / scale};
	return n;
}
