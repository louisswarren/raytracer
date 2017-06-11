typedef struct {
	double x, y, z;
} Vector;

Vector vectorsub(Vector a, Vector b);

double dot(Vector a, Vector b);

Vector normalise(Vector a);
