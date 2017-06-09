#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define EPSILON 0.001
#define print(X) printf("%f\n", X)

typedef struct {
	double x, y, z;
} Point;

Point pointsub(Point a, Point b)
{
	Point c = {a.x - b.x, a.y - b.y, a.z - b.z};
	return c;
}

double dot(Point a, Point b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

int print_point(Point a)
{
	return printf("(%f, %f, %f)\n", a.x, a.y, a.z);
}

typedef struct {
	Point center;
	double width, height, depth;
} Box;

typedef struct {
	Point center;
	double radius;
} Sphere;

double
intersect_sphere(void *x, Point pos, Point dir)
{
	Sphere *s = x;
	Point dist = pointsub(pos, s->center);
	double dp = dot(dir, dist);
	double to_rim_squared = dot(dist, dist) - s->radius * s->radius;
	double delta = dp * dp - to_rim_squared;

	// If beyond the rim
	if (fabs(delta) < EPSILON || delta < 0) {
		return -1;
	}

	double t1 = -dp - sqrt(delta);
	double t2 = -dp + sqrt(delta);
	if (fabs(t1) < EPSILON) {
		if (t2 > 0) {
			return t2;
		} else {
			t1 = -1;
		}
	}
	if (fabs(t2) < EPSILON)
		t2 = -1;

	return (t1 < t2) ? t1 : t2;
}

typedef double intersector(void *, Point, Point);

typedef struct {
	void *drawable;
	intersector *intersect;
} Object;


Point
normalise(Point a)
{
	double scale = sqrt(dot(a, a));
	Point n = {a.x / scale, a.y / scale, a.z / scale};
	return n;
}

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

