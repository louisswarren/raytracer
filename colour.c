#include <math.h>
#include <stdlib.h>

#include "colour.h"

Colour colour_combine(Colour *this, Colour *other, double other_scale)
{
	return (Colour){
		this->r + other->r * other_scale,
		this->g + other->g * other_scale,
		this->b + other->b * other_scale,
	};
}

Colour colour_interpolate(Colour *this, Colour *other, double t)
{
	return (Colour){
		this->r * (1 - t) + other->r * t,
		this->g * (1 - t) + other->g * t,
		this->b * (1 - t) + other->b * t,
	};
}

Colour colour_phong(Colour *mat, Colour *amb, double diff, double spec)
{
	return (Colour){
		(amb->r + diff) * mat ->r + spec,
		(amb->g + diff) * mat ->g + spec,
		(amb->b + diff) * mat ->b + spec,
	};
}

double colour_variance(Colour *expected, Colour samples[], size_t n)
{
	double sum = 0;
	for (size_t i = 0; i < n; i++) {
		sum += pow(expected->r - samples[i].r, 2);
		sum += pow(expected->g - samples[i].g, 2);
		sum += pow(expected->b - samples[i].b, 2);
	}
	return sum / (double)(3 * n);
}
