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


typedef double intersector(void *, Vector, Vector);
typedef Vector normal_func(void *, Vector);

typedef struct {
	void *drawable;
	intersector *intersect;
	normal_func *normal;
} Object;


static Color COLOR_RED = {1, 0, 0};
static Color COLOR_GREEN = {0, 1, 0};
static Color COLOR_BLUE = {0, 0, 1};


static Object scene[100];
static size_t scene_ctr = 0;

int print_vector(Vector a)
{
	return printf("(%f, %f, %f)\n", a.x, a.y, a.z);
}


double find_closest(Vector pos, Vector dir, size_t *closest)
{
	void *drawable;
	intersector *intersect;
	double t;
	double min_t = -1;
	size_t argmin_t;
	for (size_t i = 0; i < scene_ctr; i++) {
		drawable = scene[i].drawable;
		intersect = scene[i].intersect;
		t = intersect(drawable, pos, dir);
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
	Vector light = {0, 100, 0};
	size_t closest_index;
	Object *closest;
	Shape *shape;
	double t = find_closest(pos, dir, &closest_index);
	if (t <= 0)
		return color;

	Vector q = vectorsub(pos, vectorscale(dir, -t));
	closest = &scene[closest_index];
	shape = closest->drawable;

	color = shape->color;
	Vector objn = closest->normal(closest->drawable, q);
	double diffuse = dot(normalise(light), objn);
	return phong(color, ambient, diffuse, 0);
}


void draw()
{
	Vector eye = {0, 0, -6};
	int halfres = 200;
	Color *frame = malloc(halfres * halfres * 4 * sizeof(Color));
	if (!frame) {
		puts("Out of memory");
		return;
	}
	for (int w = -halfres; w < halfres; w++) {
		for (int h = -halfres; h < halfres; h++) {
			double x = (double) w / halfres;
			double y = (double) h / halfres;
			Vector pix = {x, y, -5};
			Vector dir = normalise(vectorsub(pix, eye));
			size_t pt = (halfres - h - 1) * halfres * 2 + (halfres + w);
			frame[pt] = trace(eye, dir);
		}
	}
	FILE *f = fopen("output.bmp", "wb");
	writebitmap(f, frame, halfres * 2, halfres * 2);
	fclose(f);
	free(frame);
}


#define add_sphere(C, X, Y, Z, R) scene[scene_ctr++] = \
	(Object){&(Sphere){C, {X, Y, Z}, R}, &intersect_sphere, &normal_sphere}

#define add_plane(C, X, Y, Z, U1, U2, U3, W1, W2, W3) scene[scene_ctr++] = \
	(Object){&(Plane){C, {X, Y, Z}, {U1, U2, U3}, {W1, W2, W3}}, \
	&intersect_plane, &normal_plane}

#define add_coplane(C, X, Y, Z, U1, U2, U3, W1, W2, W3) scene[scene_ctr++] = \
	(Object){&(Plane){C, {X, Y, Z}, {U1, U2, U3}, {W1, W2, W3}}, \
	&intersect_coplane, &normal_plane}

#define add_infinite_plane(C, X, Y, Z, U1, U2, U3, W1, W2, W3) \
	scene[scene_ctr++] = \
	(Object){&(Plane){C, {X, Y, Z}, {U1, U2, U3}, {W1, W2, W3}}, \
	                  &intersect_coplane, &normal_plane}

int main(void)
{
	Color wallcolor = {1, 0.8, 0.4};
	Color floorcolor = {0.3, 0.3, 0.35};

	add_sphere(COLOR_RED,     0, 0, 10,    2);
	add_sphere(COLOR_BLUE,    4, 4, 5,     1);

	float d = 10;
	float h = 2;
	float r = d - h;

	add_infinite_plane(wallcolor,     0,  0, d*2,    1, 0, 0,    0, 1, 0);
	add_infinite_plane(COLOR_RED,    -d,  0,   0,    0, 0, 1,    0, 1, 0);
	add_infinite_plane(COLOR_BLUE,    d,  0,   0,    0, 0, 1,    0, 1, 0);
	add_infinite_plane(floorcolor,    0, -d,   0,    1, 0, 0,    0, 0, 1);

	add_coplane(floorcolor,    -h, d, r,    h*2, 0, 0,    0, 0, h*2);

	draw();
}
