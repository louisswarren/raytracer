typedef struct {
	Color color;
} Shape;

typedef struct {
	Color color;
	Vector center;
	double width, height, depth;
} Box;

typedef struct {
	Color color;
	Vector center;
	double radius;
} Sphere;


double intersect_box(void *x, Vector pos, Vector dir);
double intersect_sphere(void *x, Vector pos, Vector dir);
