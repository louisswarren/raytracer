typedef struct {
	Vector center;
	double radius;
} Sphere;

typedef struct {
	Vector anchor;
	Vector dir1, dir2;
} Plane;

Vector sphere_normal(Sphere *s, Vector *pos);

double sphere_intersect(Sphere *s, Ray *ray);

Vector plane_normal(Plane *p, Vector *pos);

double infplane_intersect(Plane *p, Ray *ray);

double plane_intersect(Plane *p, Ray *ray);

