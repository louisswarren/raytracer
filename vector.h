typedef struct {
	double x, y, z;
} Vector;

Vector vectorsub(Vector a, Vector b);

Vector vectorscale(Vector a, double s);

double dot(Vector a, Vector b);

Vector normalise(Vector a);

Vector cross(Vector a, Vector b);
