typedef struct {
	double x, y, z;
} Vector;

typedef struct {
	Vector pos;
	Vector dir;
} Ray;

Vector vecadd(Vector a, Vector b);

Vector vecsub(Vector a, Vector b);

Vector vecscale(Vector a, double s);

double vecdot(Vector a, Vector b);

double vecmagnitude(Vector a);

Vector vecnormalise(Vector a);

Vector veccross(Vector a, Vector b);
