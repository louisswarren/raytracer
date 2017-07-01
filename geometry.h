typedef struct {
	Vector center;
	double radius;
} Sphere;

typedef struct {
	Vector anchor;
	Vector dir1, dir2;
} Plane;

Vector sphere_normal(void *x, Vector *pos);

double sphere_intersect(void *x, Ray *ray);

Vector plane_normal(void *x, Vector *pos);

double infplane_intersect(void *x, Ray *ray);

double plane_intersect(void *x, Ray *ray);

