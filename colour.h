typedef struct {
	double r, g, b;
} Colour;

Colour colour_combine(Colour *this, Colour *other, double other_scale);

Colour colour_interpolate(Colour *this, Colour *other, double t);

Colour colour_phong(Colour *mat, Colour *amb, double diff, double spec);

double colour_variance(Colour *expected, Colour samples[], size_t n);
