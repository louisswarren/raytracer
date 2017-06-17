#define _POSIX_C_SOURCE 200809L
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
	intersect_func *intersect;
	normal_func *normal;
} Scenery;


static Scenery scene[100];
static size_t scene_ctr = 0;


double find_closest(Vector pos, Vector dir, size_t *closest)
{
	void *shape;
	intersect_func *intersect;
	double t;
	double min_t = -1;
	size_t argmin_t;
	for (size_t i = 0; i < scene_ctr; i++) {
		shape = scene[i].shape;
		intersect = scene[i].intersect;
		t = intersect(shape, pos, dir);
		if (t > 0 && (min_t < 0 || t < min_t)) {
			min_t = t;
			argmin_t = i;
		}
	}
	*closest = argmin_t;
	return min_t;
}


Color trace(Vector pos, Vector dir)
{
	Color ambient = {0.2, 0.2, 0.2};
	Color color = {0, 0, 0};
	Vector light = {-5, 5, 0};
	size_t closest_index;
	Scenery *closest;
	Shape *shape;
	double t = find_closest(pos, dir, &closest_index);
	if (t <= 0)
		return color;

	Vector q = vecadd(pos, vecscale(dir, t));
	closest = &scene[closest_index];
	shape = closest->shape;

	color = shape->color;
	Vector objn = closest->normal(shape, q);
	Vector lightdir = vecnormalise(vecsub(light, q));
	double diffuse = vecdot(lightdir, objn);
	double spec = 0;
	if (diffuse <= 0) {
		return phong(color, ambient, 0, 0);
	}
	double shadow_t = find_closest(q, lightdir, &closest_index);
	if (shadow_t > 0 && shadow_t < vecnorm(vecsub(light, q))) {
		return phong(color, ambient, 0, 0);
	}
	Vector specdir = vecsub(vecscale(objn, 2 * diffuse), lightdir);
	double specscale = vecdot(vecnormalise(specdir), vecscale(dir, -1));
	if (specscale < 0)
		spec = pow(specscale, 30);
	return phong(color, ambient, diffuse, spec);
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
			frame[pt] = trace(eye, dir);
		}
	}
}


#define add_sphere(C, X, Y, Z, R) scene[scene_ctr++] = \
	(Scenery){&(Sphere){C, {X, Y, Z}, R}, &intersect_sphere, &normal_sphere}

#define add_plane(C, X, Y, Z, U1, U2, U3, W1, W2, W3) scene[scene_ctr++] = \
	(Scenery){&(Plane){C, {X, Y, Z}, {U1, U2, U3}, {W1, W2, W3}}, \
	&intersect_plane, &normal_plane}

#define add_coplane(C, X, Y, Z, U1, U2, U3, W1, W2, W3) scene[scene_ctr++] = \
	(Scenery){&(Plane){C, {X, Y, Z}, {U1, U2, U3}, {W1, W2, W3}}, \
	&intersect_coplane, &normal_plane}

#define add_infinite_plane(C, X, Y, Z, U1, U2, U3, W1, W2, W3) \
	scene[scene_ctr++] = \
	(Scenery){&(Plane){C, {X, Y, Z}, {U1, U2, U3}, {W1, W2, W3}}, \
	&intersect_coplane, &normal_plane}


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

	add_sphere(COLOR_RED,     0, 0, 10,    2);
	add_sphere(COLOR_BLUE,    4, 4, 5,     1);

	float d = 10;
	float h = 2;
	float r = d - h;

	add_infinite_plane(wallcolor,     0,  0, d*2,    0, 1, 0,    1, 0, 0);
	add_infinite_plane(COLOR_RED,    -d,  0,   0,    0, 1, 0,    0, 0, 1);
	add_infinite_plane(COLOR_BLUE,    d,  0,   0,    0, 1, 1,    0, 1, 0);
	add_infinite_plane(floorcolor,    0, -d,   0,    0, 0, 1,    1, 0, 0);

	add_coplane(floorcolor,    -h, d, r,    h*2, 0, 0,    0, 0, h*2);

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
