typedef double intersect_func(void *, Ray *);
typedef Vector normal_func(void *, Vector *);
typedef Colour colour_func(Vector, Colour, void *);

typedef struct {
	void *shape;
	Colour colour;
	double refl;
	intersect_func *intersect;
	normal_func *normal;
	colour_func *colourer;
	void * colour_params;
} Scenery;

typedef struct {
	Scenery *object;
	double dist;
	Vector pos;
} Observation;


Observation scene_observe(Ray ray);

Scenery *add_sphere(Colour, double, double, double, double, double);
Scenery *add_plane(Colour, double, double, double, double, double, double, double, double, double, double);
Scenery *add_infplane(Colour, double, double, double, double, double, double, double, double, double, double);
