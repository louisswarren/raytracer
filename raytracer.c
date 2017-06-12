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

Color COLOR_RED = {1, 0, 0};
Color COLOR_GREEN = {0, 1, 0};
Color COLOR_BLUE = {0, 0, 1};

int
print_vector(Vector a)
{
	return printf("(%f, %f, %f)\n", a.x, a.y, a.z);
}

#define print(X) printf("%f\n", X)

typedef double intersector(void *, Vector, Vector);
typedef Vector normal_func(void *, Vector);

typedef struct {
	void *drawable;
	intersector *intersect;
	normal_func *normal;
} Object;

static Object scene[100];
static size_t scene_ctr = 0;

#define add_sphere(X) \
	scene[scene_ctr++] = (Object){&X, &intersect_sphere, &normal_sphere}
#define add_plane(X) \
	scene[scene_ctr++] = (Object){&X, &intersect_plane, &normal_plane}
#define add_coplane(X) \
	scene[scene_ctr++] = (Object){&X, &intersect_coplane, &normal_plane}
#define add_infinite_plane(X) \
	scene[scene_ctr++] = (Object){&X, &intersect_infinite_plane, &normal_plane}




double
find_closest(Vector pos, Vector dir, size_t *closest)
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

void
draw()
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

int
main(void)
{
	Color wallcolor = {1, 0.8, 0.4};
	Color floorcolor = {0.3, 0.3, 0.35};

	Sphere s1 = {COLOR_RED, {0, 0, 10}, 2};
	Sphere s2 = {COLOR_BLUE, {4, 4, 5}, 1};
	add_sphere(s1);
	add_sphere(s2);

	float d = 10;
	float h = 2;
	float r = d - h;

	Plane wall_back =   {wallcolor,  {0, 0, d*2}, {1, 0, 0}, {0, 1, 0}};
	Plane wall_left =   {COLOR_RED,  {-d, 0, 0},  {0, 0, 1}, {0, 1, 0}};
	Plane wall_right =  {COLOR_BLUE, {d, 0, 0},   {0, 0, 1}, {0, 1, 0}};
	Plane wall_bottom = {floorcolor, {0, -d, 0},  {1, 0, 0}, {0, 0, 1}};
	add_infinite_plane(wall_back);
	add_infinite_plane(wall_left);
	add_infinite_plane(wall_right);
	add_infinite_plane(wall_bottom);

	Plane wall_top =    {floorcolor, {-h, d, r},  {h*2, 0, 0}, {0, 0, h*2}};
	add_coplane(wall_top);

	draw();
}

