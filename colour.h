struct colour {
	double r, g, b;
};

void addc(struct colour *this, const struct colour *other, double t);
void mixc(struct colour *this, const struct colour *other, double t);
void phongc(struct colour *mat, const struct colour *amb, double diff, double spec);
double colour_variance(const struct colour *x, const struct colour samples[], size_t n);
