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

typedef struct {
	void *drawable;
	intersector *intersect;
} Object;

double
find_closest(Object *scene, size_t n, Vector pos, Vector dir, size_t *closest)
{
	void *drawable;
	intersector *intersect;
	double t;
	double min_t = -1;
	size_t argmin_t;
	for (size_t i = 0; i < n; i++) {
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

Color trace(Object *scene, size_t n, Vector pos, Vector dir)
{
	Color color = {0, 0, 0};
	size_t closest;
	if (find_closest(scene, n, pos, dir, &closest) > 0) {
		color = ((Shape *)scene[closest].drawable)->color;
	}
	return color;
}

void
draw(Object *scene, size_t n)
{
	Vector eye = {0, 0, -6};
	Vector light = {0, 0, 100};
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
			frame[pt] = trace(scene, n, eye, dir);
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

	float d = 10;
	float h = 2;
	float r = d - h;

	Plane wall_back =   {wallcolor,  {0, 0, d*2}, {1, 0, 0}, {0, 1, 0}};
	Plane wall_left =   {COLOR_RED,  {-d, 0, 0},  {0, 0, 1}, {0, 1, 0}};
	Plane wall_right =  {COLOR_BLUE, {d, 0, 0},   {0, 0, 1}, {0, 1, 0}};
	Plane wall_bottom = {floorcolor, {0, -d, 0},  {1, 0, 0}, {0, 0, 1}};

	Plane wall_top =    {floorcolor, {-h, d, r},  {h*2, 0, 0}, {0, 0, h*2}};

	Object scene[] = {
		{&s1, &intersect_sphere},
		{&s2, &intersect_sphere},
		{&wall_back,   &intersect_infinite_plane},
		{&wall_left,   &intersect_infinite_plane},
		{&wall_right,  &intersect_infinite_plane},
		{&wall_bottom, &intersect_infinite_plane},
		{&wall_top,    &intersect_coplane},
	};
	draw(scene, 7);
}

