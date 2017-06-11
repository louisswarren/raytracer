typedef struct {
	Color color;
} Shape;

typedef struct {
	Color color;
	Vector anchor;
	Vector dir1, dir2;
} Plane;

typedef struct {
	Color color;
	Vector center;
	Vector axis1, axis2, axis3;
	double width, height, depth;
} Box;

typedef struct {
	Color color;
	Vector center;
	double radius;
} Sphere;


double intersect_sphere(void *x, Vector pos, Vector dir);

double intersect_plane(void *x, Vector pos, Vector dir);

double intersect_coplane(void *x, Vector pos, Vector dir);

double intersect_infinite_plane(void *x, Vector pos, Vector dir);

Vector normal_plane(Plane *p, Vector pos);
