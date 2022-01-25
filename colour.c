#include <math.h>
#include <stdlib.h>

#include "colour.h"

void
colour_combine(Colour *this, Colour other, double other_scale)
{
	this->r += other.r * other_scale;
	this->g += other.g * other_scale;
	this->b += other.b * other_scale;
}

void
colour_interpolate(Colour *this, Colour other, double t)
{
	this->r = this->r * (1 - t) + other.r * t;
	this->g = this->g * (1 - t) + other.g * t;
	this->b = this->b * (1 - t) + other.b * t;
}

void
colour_phong(Colour *col, Colour amb, double diff, double spec)
{
	col->r = col->r * (amb.r + diff) + spec;
	col->g = col->g * (amb.g + diff) + spec;
	col->b = col->b * (amb.b + diff) + spec;
}

double
colour_variance(Colour expected, Colour *samples, size_t n)
{
	double sum = 0;
	for (size_t i = 0; i < n; i++) {
		sum += pow(expected.r - samples[i].r, 2);
		sum += pow(expected.g - samples[i].g, 2);
		sum += pow(expected.b - samples[i].b, 2);
	}
	return sum / (3 * n);
}
