typedef struct {
	Vector center;
	double radius;
} Sphere;

typedef struct {
	Vector anchor;
	Vector dir1, dir2;
} Plane;

typedef struct {
	Vector center;
	Vector axis1, axis2, axis3;
	double width, height, depth;
} Box;


double intersect_sphere(void *x, Vector pos, Vector dir);

Vector normal_sphere(void *x, Vector pos);


double intersect_plane(void *x, Vector pos, Vector dir);

double intersect_coplane(void *x, Vector pos, Vector dir);

double intersect_infinite_plane(void *x, Vector pos, Vector dir);

Vector normal_plane(void *x, Vector pos);
