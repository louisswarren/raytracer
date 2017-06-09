#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "point.h"
#include "geometry.h"

int
print_point(Point a)
{
	return printf("(%f, %f, %f)\n", a.x, a.y, a.z);
}

#define print(X) printf("%f\n", X)

typedef double intersector(void *, Point, Point);

typedef struct {
	void *drawable;
	intersector *intersect;
} Object;

double
trace(Object *scene, size_t n, Point pos, Point dir, size_t *closest)
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
	Point eye = {0, 0, -1};
	int halfres = 25;
	size_t closest;
	for (int w = -halfres; w <= halfres; w++) {
		for (int h = -halfres; h <= halfres; h++) {
			double x = (double) w / halfres;
			double y = (double) h / halfres;
			Point pix = {x, y, 0};
			Point dir = normalise(pointsub(pix, eye));
			if (trace(scene, n, eye, dir, &closest) > 0) {
				printf("#");
			} else {
				printf(".");
			}
		}
		printf("\n");
	}

}


int
main(void)
{
	Sphere s1 = {{0, 0, 10}, 2};
	Sphere s2 = {{4, 4, 5}, 1};
	Object scene[] = {
		{&s1, &intersect_sphere},
		{&s2, &intersect_sphere},
	};
	draw(scene, 2);
}

