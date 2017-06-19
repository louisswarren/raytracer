#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "vector.h"
#include "color.h"
#include "geometry.h"
#include "writebmp.h"


typedef double intersect_func(void *, Vector, Vector);
typedef Vector normal_func(void *, Vector);

typedef struct {
	void *shape;
	double refl;
	intersect_func *intersect;
	normal_func *normal;
} Scenery;

typedef struct {
	Scenery *object;
	double dist;
	Vector pos;
} Observation;


static Scenery scene[100];
static size_t scene_ctr = 0;

static Color ambient_light = {0.2, 0.2, 0.2};
static Vector light_pos = {5, 5, 0};

Observation find_closest(Ray ray)
{
	size_t argmin_dist;
	double dist, min_dist = -1;
	for (size_t i = 0; i < scene_ctr; i++) {
		dist = scene[i].intersect(scene[i].shape, ray.pos, ray.dir);
		if (dist > 0 && (min_dist < 0 || dist < min_dist)) {
			min_dist = dist;
			argmin_dist = i;
		}
	}
	Vector pos = vecadd(ray.pos, vecscale(ray.dir, min_dist));
	return (Observation){&scene[argmin_dist], min_dist, pos};
}

int in_shadow(Vector pos, Vector lightdir)
{
	Ray shadow_ray = {pos, lightdir};
	Observation shade = find_closest(shadow_ray);
	double dist_to_light = vecnorm(vecsub(light_pos, pos));
	return (shade.dist > 0 && shade.dist < dist_to_light);
}

Color trace(Ray ray)
{
	Observation observed = find_closest(ray);
	if (observed.dist <= 0)
		return (Color){0, 0, 0};

	Shape *shape = observed.object->shape;
	Color color = shape->color;
	Vector objn = observed.object->normal(shape, observed.pos);

	Vector lightdir = vecnormalise(vecsub(light_pos, observed.pos));
	double diffuse = vecdot(lightdir, objn);
	double spec = 0;
	if (diffuse <= 0) {
		return phong(color, ambient_light, 0, 0);
	}

	if (in_shadow(observed.pos, lightdir)) {
		return phong(color, ambient_light, 0, 0);
	}
	Vector specdir = vecsub(vecscale(objn, 2 * diffuse), lightdir);
	double specscale = vecdot(vecnormalise(specdir), vecscale(ray.dir, -1));
	if (specscale < 0)
		spec = pow(specscale, 30);
	return phong(color, ambient_light, diffuse, spec);
}


void draw(Color *frame, Vector eye, double focal, size_t width, size_t height)
{
	double w_ratio = width < height ? (double) width / height : 1;
	double h_ratio = height < width ? (double) height / width : 1;
	int halfwidth = width / 2;
	int halfheight = height / 2;

	for (int w = -halfwidth; w < halfwidth; w++) {
		for (int h = -halfheight; h < halfheight; h++) {
			double x = (double) w_ratio * w / halfwidth;
			double y = (double) h_ratio * h / halfheight;
			Vector view_dir = {x, y, focal};
			Vector pix = vecadd(eye, view_dir);
			Vector dir = vecnormalise(view_dir);
			size_t pt = (halfheight - h - 1) * width + (halfwidth + w);
			Ray ray = {eye, dir};
			frame[pt] = trace(ray);
		}
	}
}


#define add_sphere(C, Q, X, Y, Z, R) scene[scene_ctr++] = \
	(Scenery){&(Sphere){C, {X, Y, Z}, R}, Q, &intersect_sphere, &normal_sphere}

#define add_plane(C, Q, X, Y, Z, U1, U2, U3, W1, W2, W3) scene[scene_ctr++] = \
	(Scenery){&(Plane){C, {X, Y, Z}, {U1, U2, U3}, {W1, W2, W3}}, \
	Q, &intersect_plane, &normal_plane}

#define add_coplane(C, Q, X, Y, Z, U1, U2, U3, W1, W2, W3) scene[scene_ctr++] = \
	(Scenery){&(Plane){C, {X, Y, Z}, {U1, U2, U3}, {W1, W2, W3}}, \
	Q, &intersect_coplane, &normal_plane}

#define add_infinite_plane(C, Q, X, Y, Z, U1, U2, U3, W1, W2, W3) \
	scene[scene_ctr++] = \
	(Scenery){&(Plane){C, {X, Y, Z}, {U1, U2, U3}, {W1, W2, W3}}, \
	Q, &intersect_infinite_plane, &normal_plane}


int print_vector(Vector a)
{
	return printf("(%f, %f, %f)\n", a.x, a.y, a.z);
}

int main(void)
{
	Color COLOR_RED = {1, 0, 0};
	Color COLOR_GREEN = {0, 1, 0};
	Color COLOR_BLUE = {0, 0, 1};
	Color wallcolor = {1, 0.8, 0.4};
	Color floorcolor = {0.3, 0.3, 0.35};

	add_sphere(COLOR_RED,   0,    0, -3, 10,    2);
	add_sphere(COLOR_GREEN, 0,    4,  4, 5,     1);

	float d = 10;
	float h = 2;
	float r = d - h;

	add_infinite_plane(wallcolor,  0,    0,  0, d*2,    0, 1, 0,    1, 0, 0);
	add_infinite_plane(COLOR_RED,  0,   -d,  0,   0,    0, 1, 0,    0, 0, 1);
	add_infinite_plane(COLOR_BLUE, 0,    d,  0,   0,    0, 1, 1,    0, 1, 0);
	add_infinite_plane(floorcolor, 0.5,  0, -d,   0,    0, 0, 1,    1, 0, 0);

	add_coplane(floorcolor, 0.2,    -h, d, r,    h*2, 0, 0,    0, 0, h*2);

	Vector eye = {0, 0, -6};
	size_t width = 480;
	size_t height = 480;

	Color *frame = malloc(width * height * sizeof(Color));
	if (!frame) {
		puts("Out of memory");
		return -1;
	}

	draw(frame, eye, 1, width, height);

	FILE *f = fopen("output.bmp", "wb");
	writebitmap(f, frame, width, height);
	fclose(f);
	free(frame);
}
