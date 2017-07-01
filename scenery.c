#include <stdlib.h>

#include "colour.h"
#include "vector.h"
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

