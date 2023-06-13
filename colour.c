#include <math.h>
#include <stdlib.h>

#include "colour.h"

void
addc(struct colour *this, const struct colour *other, double t)
{
	this->r += other->r * t;
	this->g += other->g * t;
	this->b += other->b * t;
}

void
mixc(struct colour *this, const struct colour *other, double t)
{
	this->r *= 1 - t;
	this->g *= 1 - t;
	this->b *= 1 - t;
	addc(this, other, t);
}

void
phongc(struct colour *mat, const struct colour *amb, double diff, double spec)
{
	mat->r *= amb->r + diff; mat->r += spec;
	mat->g *= amb->g + diff; mat->g += spec;
	mat->b *= amb->b + diff; mat->b += spec;
}

double
colour_variance(const struct colour *x, const struct colour samples[], size_t n)
{
	double sum = 0;
	for (size_t i = 0; i < n; i++) {
		sum += pow(x->r - samples[i].r, 2);
		sum += pow(x->g - samples[i].g, 2);
		sum += pow(x->b - samples[i].b, 2);
	}
	return sum / (3.0 * (double)n);
}
