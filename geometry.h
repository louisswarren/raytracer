typedef struct {
	Point center;
	double width, height, depth;
} Box;

typedef struct {
	Point center;
	double radius;
} Sphere;


double intersect_box(void *x, Point pos, Point dir);
double intersect_sphere(void *x, Point pos, Point dir);
