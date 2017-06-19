#include <math.h>
#include <stdlib.h>

#include "color.h"
#include "vector.h"
#include "geometry.h"

static const double epsilon = 0.001;

double intersect_sphere(void *x, Vector pos, Vector dir)
{
	Sphere *s = x;
	Vector dist = vecsub(pos, s->center);
	double dp = vecdot(dir, dist);
	double to_rim_squared = vecdot(dist, dist) - s->radius * s->radius;
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

Vector normal_sphere(void *x, Vector pos)
{
	Sphere *s = x;
	return vecnormalise(vecsub(pos, s->center));
}

double intersect_infinite_plane(void *x, Vector pos, Vector dir)
{
	Plane *p = x;
	Vector n = normal_plane(p, pos);
	Vector dist = vecsub(p->anchor, pos);
	double dp = vecdot(dir, n);
	if (fabs(dp) < epsilon)
		return -1;
	double t = vecdot(dist, n) / dp;
	if (fabs(t) < epsilon)
		return -1;
	return t;
}

double intersect_plane(void *x, Vector pos, Vector dir)
{
	Plane *p = x;
	double t = intersect_infinite_plane(p, pos, dir);

	Vector hdir = vecsub(vecadd(pos, vecscale(dir, t)), p->anchor);
	double proj1_scale = vecdot(hdir, p->dir1);
	if (proj1_scale < 0 || proj1_scale > vecdot(p->dir1, p->dir1))
		return -1;
	double proj2_scale = vecdot(hdir, p->dir2);
	if (proj2_scale < 0 || proj2_scale > vecdot(p->dir2, p->dir2))
		return -1;
	return t;
}

double intersect_coplane(void *x, Vector pos, Vector dir)
{
	Plane *p = x;
	double t = intersect_infinite_plane(p, pos, dir);

	Vector hdir = vecsub(vecadd(pos, vecscale(dir, t)), p->anchor);
	double proj1_scale = vecdot(hdir, p->dir1);
	if (proj1_scale < 0 || proj1_scale > vecdot(p->dir1, p->dir1))
		return t;
	double proj2_scale = vecdot(hdir, p->dir2);
	if (proj2_scale < 0 || proj2_scale > vecdot(p->dir2, p->dir2))
		return t;
	return -1;
}

Vector normal_plane(void *x, Vector pos)
{
	Plane *p = x;
	return vecnormalise(veccross(p->dir1, p->dir2));
}
