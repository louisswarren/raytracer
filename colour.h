typedef struct {
	double r, g, b;
} Colour;

void colour_combine(Colour *this, Colour other, double other_scale);

void colour_interpolate(Colour *this, Colour other, double t);

void colour_phong(Colour *col, Colour amb, double diff, double spec);

double colour_variance(Colour expected, Colour samples[], size_t n);
