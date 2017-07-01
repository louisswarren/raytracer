#include <math.h>
#include <stdlib.h>

#include "vector.h"
#include "geometry.h"

static const double epsilon = 0.001;


Vector sphere_normal(Sphere *s, Vector *pos)
{
	Vector normal = vec_sub(pos, &s->center);
	vec_normalise(&normal);
	return normal;
}


double sphere_intersect(Sphere *s, Ray *ray)
{
	Vector dist = vec_sub(&ray->pos, &s->center);
	double rim_dist_squared = vec_dot(&dist, &dist) - s->radius * s->radius;
	double intersect_dist = vec_dot(&ray->dir, &dist);
	double delta = intersect_dist * intersect_dist - rim_dist_squared;

	// If beyond the rim
	if (fabs(delta) < epsilon || delta < 0) {
		return -1;
	}

	double t1 = -intersect_dist - sqrt(delta);
	double t2 = -intersect_dist + sqrt(delta);
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

Vector plane_normal(Plane *p, Vector *pos)
{
	Vector normal = vec_cross(&p->dir1, &p->dir2);
	vec_normalise(&normal);
	return normal;
}

double infplane_intersect(Plane *p, Ray *ray)
{
	Vector n = plane_normal(p, &ray->pos);
	Vector dist = vec_sub(&p->anchor, &ray->pos);
	double dp = vec_dot(&ray->dir, &n);
	if (fabs(dp) < epsilon)
		return -1;
	double t = vec_dot(&dist, &n) / dp;
	if (fabs(t) < epsilon)
		return -1;
	return t;
}

double plane_intersect(Plane *p, Ray *ray)
{
	double t = infplane_intersect(p, ray);
	if (t < 0)
		return -1;

	Vector hit = ray_at_param(ray, t);
	Vector hdir = vec_sub(&hit, &p->anchor);
	double proj1_scale = vec_dot(&ray->dir, &p->dir1);
	if (proj1_scale < 0 || proj1_scale > vec_dot(&p->dir1, &p->dir1))
		return -1;
	double proj2_scale = vec_dot(&hdir, &p->dir2);
	if (proj2_scale < 0 || proj2_scale > vec_dot(&p->dir2, &p->dir2))
		return -1;
	return t;
}
