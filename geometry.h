struct geo_sphere {
	struct vector center;
	double radius;
};
struct vector geo_sphere_normal(const void *x, const struct vector *pos);
double geo_sphere_intersect(const void *x, const struct ray *ray);

struct geo_plane {
	struct vector anchor;
	struct vector dir1, dir2;
};
struct vector geo_plane_normal(const void *x, const struct vector *pos);
double geo_infplane_intersect(const void *x, const struct ray *ray);
double geo_plane_intersect(const void *x, const struct ray *ray);

