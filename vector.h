struct vector {
	double x, y, z;
};

struct ray {
	struct vector pos;
	struct vector dir;
};

double dotv(const struct vector *u, const struct vector *v);
double normv(const struct vector *u);

void addv(struct vector *u, const struct vector *v);
void subv(struct vector *u, const struct vector *v);
void mulv(struct vector *u, double s);

void normalisev(struct vector *u);
void crossv(struct vector *u, const struct vector *v);
void alongv(struct vector *u, const struct vector *v, double t);
