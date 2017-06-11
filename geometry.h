typedef struct {
	Vector center;
	double width, height, depth;
} Box;

typedef struct {
	Vector center;
	double radius;
} Sphere;


double intersect_box(void *x, Vector pos, Vector dir);
double intersect_sphere(void *x, Vector pos, Vector dir);
