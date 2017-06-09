#include <math.h>

#include "point.h"
#include "geometry.h"

static const double epsilon = 0.001;

double
intersect_sphere(void *x, Point pos, Point dir)
{
	Sphere *s = x;
	Point dist = pointsub(pos, s->center);
	double dp = dot(dir, dist);
	double to_rim_squared = dot(dist, dist) - s->radius * s->radius;
	double delta = dp * dp - to_rim_squared;

	// If beyond the rim
	if (fabs(delta) < epsilon || delta < 0) {
		return -1;
	}

	double t1 = -dp - sqrt(delta);
	double t2 = -dp + sqrt(delta);
	if (fabs(t1) < epsilon) {
		if (t2 > 0) {
			return t2;
		} else {
			t1 = -1;
		}
	}
	if (fabs(t2) < epsilon)
		t2 = -1;

	return (t1 < t2) ? t1 : t2;
}

