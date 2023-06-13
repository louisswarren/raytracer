#include <math.h>

#include "vector.h"

void
addv(struct vector *u, const struct vector *v)
{
	u->x += v->x;
	u->y += v->y;
	u->z += v->z;
}

void
subv(struct vector *u, const struct vector *v)
{
	u->x -= v->x;
	u->y -= v->y;
	u->z -= v->z;
}

void
mulv(struct vector *u, double s)
{
	u->x *= s;
	u->y *= s;
	u->z *= s;
}

void
alongv(struct vector *u, const struct vector *v, double t)
{
	u->x += v->x * t;
	u->y += v->y * t;
	u->z += v->z * t;
}

double
dotv(const struct vector *u, const struct vector *v)
{
	return (u->x * v->x) +
	       (u->y * v->y) +
	       (u->z * v->z);
}

double
normv(const struct vector *u)
{
	return sqrt(dotv(u, u));
}

void
normalisev(struct vector *u)
{
	mulv(u, 1.0 / normv(u));
}

void
crossv(struct vector *u, const struct vector *v)
{
	u->x = u->y * v->z - u->z * v->y;
	u->y = u->z * v->x - u->x * v->z;
	u->z = u->x * v->y - u->y * v->x;
}
