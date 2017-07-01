#include <stdlib.h>

#include "colour.h"
#include "vector.h"
#include "geometry.h"
#include "scenery.h"

Scenery scene[100];
size_t scene_ctr = 0;

Observation scene_observe(Ray ray)
{
	size_t argmin_dist;
	double dist, min_dist = -1;
	for (size_t i = 0; i < scene_ctr; i++) {
		dist = scene[i].intersect(scene[i].shape, &ray);
		if (dist > 0 && (min_dist < 0 || dist < min_dist)) {
			min_dist = dist;
			argmin_dist = i;
		}
	}
	if (min_dist < 0)
		return (Observation){0, -1, {0, 0, 0}};
	Vector pos = ray_at_param(&ray, min_dist);
	return (Observation){&scene[argmin_dist], min_dist, pos};
}


static Colour flat_colour(Vector pos, Colour colour, void *_)
{
	return colour;
}


Scenery *add_sphere(col, refl, x, y, z, radius)
	Colour col;
	double refl;
	double x, y, z;
	double radius;
{
	Scenery *scenery = &scene[scene_ctr++];
	scenery->shape = malloc(sizeof(Sphere));

	*(Sphere *)scenery->shape = (Sphere){(Vector){x, y, z}, radius};
	scenery->colour = col;
	scenery->refl = refl;
	scenery->intersect = &sphere_intersect;
	scenery->normal = &sphere_normal;
	scenery->colourer = &flat_colour;
	scenery->colour_params = 0;
	return scenery;
}

double trivial(void *x, Ray *ray)
{
	return 1;
}


Scenery *add_plane(col, refl, x, y, z, u1, u2, u3, w1, w2, w3)
	Colour col;
	double refl;
	double x, y, z;
	double u1, u2, u3;
	double w1, w2, w3;
{
	Scenery *scenery = &scene[scene_ctr++];
	scenery->shape = malloc(sizeof(Plane));

	Vector pos = {x, y, z};
	Vector u = {u1, u2, u3};
	Vector w = {w1, w2, w3};
	*(Plane *)scenery->shape = (Plane){(Vector){x, y, z}, (Vector){u1, u2, u3}, (Vector){w1, w2, w3}};
//	(*(Plane *)scenery->shape).anchor = pos;
//	(*(Plane *)scenery->shape).dir1 = w;
//	(*(Plane *)scenery->shape).dir2 = w;
	scenery->colour = col;
	scenery->refl = refl;
	scenery->intersect = &plane_intersect;
	scenery->normal = &plane_normal;
	scenery->colourer = &flat_colour;
	scenery->colour_params = 0;
	return scenery;
}

Scenery *add_infplane(col, refl, x, y, z, u1, u2, u3, w1, w2, w3)
	Colour col;
	double refl;
	double x, y, z;
	double u1, u2, u3;
	double w1, w2, w3;
{
	Scenery *scenery = add_plane(col, refl, x, y, z, u1, u2, u3, w1, w2, w3);
	scenery->intersect = &infplane_intersect;
	return scenery;
}
