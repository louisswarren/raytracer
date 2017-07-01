typedef struct {
	double x, y, z;
} Vector;

typedef struct {
	Vector pos;
	Vector dir;
} Ray;

Vector vec_add(Vector *this, Vector *other);

Vector vec_sub(Vector *this, Vector *other);

void vec_scale(Vector *this, double s);

Vector vec_scaled(Vector *this, double s);

double vec_dot(Vector *this, Vector *other);

double vec_magnitude(Vector *this);

void vec_normalise(Vector *this);

Vector vec_normalised(Vector *this);

Vector vec_cross(Vector *this, Vector *other);
