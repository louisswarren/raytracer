#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define EPSILON 0.001

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


typedef struct {
	void *drawable;
	double (*intersect)(void *, Point, Point);
} Object;


Point normalise(Point a)
{
	double scale = sqrt(dot(a, a));
	Point n = {a.x / scale, a.y / scale, a.z / scale};
	return n;
}

void
draw(Object *scene, size_t numobjects)
{
	if (numobjects != 1) {
		puts("Can't handle that number of objects");
		return;
	}
	Point eye = {0, 0, -1};
	int halfres = 25;
	for (int w = -halfres; w <= halfres; w++) {
		for (int h = -halfres; h <= halfres; h++) {
			double x = (double) w / halfres;
			double y = (double) h / halfres;
			Point pix = {x, y, 0};
			Point dir = normalise(pointsub(pix, eye));
			void *drawable = scene[0].drawable;
			double (*intersect)(void *, Point, Point) = scene[0].intersect;
			double t = intersect(drawable, eye, dir);
			if (t > 0) {
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
	Sphere s = {{0, 0, 1}, 1};
	Object scene[] = {{&s, &intersect_sphere}};
	draw(scene, 1);
}
