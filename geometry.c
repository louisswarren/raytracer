#include <math.h>
#include <stdlib.h>

#include "vector.h"
#include "geometry.h"

static const double epsilon = 0.001;


struct vector
geo_sphere_normal(const void *x, const struct vector *pos)
{
	const struct geo_sphere *s = x;
	struct vector normal = *pos;
	subv(&normal, &s->center);
	normalisev(&normal);
	return normal;
}


double
geo_sphere_intersect(const void *x, const struct ray *ray)
{
	const struct geo_sphere *s = x;
	struct vector dist = ray->pos;
	subv(&dist, &s->center);
	double rim_dist_squared = dotv(&dist, &dist) - s->radius * s->radius;
	double intersect_dist = dotv(&ray->dir, &dist);
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

struct vector
geo_plane_normal(const void *x, const struct vector *pos)
{
	(void)pos;
	const struct geo_plane *p = x;
	struct vector normal = p->dir1;
	crossv(&normal, &p->dir2);
	normalisev(&normal);
	return normal;
}

double
geo_infplane_intersect(const void *x, const struct ray *ray)
{
	const struct geo_plane *p = x;
	struct vector n = geo_plane_normal(p, &ray->pos);
	struct vector dist = p->anchor;
	subv(&dist, &ray->pos);

	double dp = dotv(&ray->dir, &n);
	if (fabs(dp) < epsilon)
		return -1;
	double t = dotv(&dist, &n) / dp;
	if (fabs(t) < epsilon)
		return -1;
	return t;
}

double
geo_plane_intersect(const void *x, const struct ray *ray)
{
	const struct geo_plane *p = x;
	double t = geo_infplane_intersect(p, ray);
	if (t < 0)
		return -1;

	struct vector h = ray->pos;
	alongv(&h, &ray->dir, t);
	subv(&h, &p->anchor);
	double proj1_scale = dotv(&ray->dir, &p->dir1);
	if (proj1_scale < 0 || proj1_scale > dotv(&p->dir1, &p->dir1))
		return -1;
	double proj2_scale = dotv(&h, &p->dir2);
	if (proj2_scale < 0 || proj2_scale > dotv(&p->dir2, &p->dir2))
		return -1;
	return t;
}
