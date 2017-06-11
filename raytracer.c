#define _POSIX_C_SOURCE 200809L
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "color.h"
#include "vector.h"
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
trace(Object *scene, size_t n, Vector pos, Vector dir, size_t *closest)
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

void
draw(Object *scene, size_t n)
{
	Vector eye = {0, 0, -1};
	int halfres = 200;
	Color frame[160000];
	size_t closest;
	for (int w = -halfres; w < halfres; w++) {
		for (int h = -halfres; h < halfres; h++) {
			double x = (double) w / halfres;
			double y = (double) h / halfres;
			Vector pix = {x, y, 0};
			Vector dir = normalise(vectorsub(pix, eye));
			size_t pt = (halfres - h - 1) * halfres * 2 + (halfres + w);
			if (trace(scene, n, eye, dir, &closest) > 0) {
				Color objcolor = ((Shape *)scene[closest].drawable)->color;
				frame[pt] = objcolor;
			} else {
				frame[pt].red = 0;
				frame[pt].green = 0;
				frame[pt].blue = 0;
			}
		}
	}
	FILE *f = fopen("output.bmp", "wb");
	writebitmap(f, frame, halfres * 2, halfres * 2);
	fclose(f);
}


int
main(void)
{
	Sphere s1 = {COLOR_RED, {0, 0, 10}, 2};
	Sphere s2 = {COLOR_BLUE, {4, 4, 5}, 1};
	Object scene[] = {
		{&s1, &intersect_sphere},
		{&s2, &intersect_sphere},
	};
	draw(scene, 2);
}

