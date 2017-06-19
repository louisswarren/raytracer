#include <math.h>
#include <stdlib.h>

#include "color.h"

Color coloradd(Color a, Color b, double b_scale)
{
	return (Color){a.red   + b.red * b_scale,
	               a.green + b.green * b_scale,
	               a.blue  + b.blue * b_scale};
}

Color phong(Color material, Color ambient, double diffuse, double specular)
{
	return (Color){(ambient.red   + diffuse) * material.red   + specular,
	               (ambient.green + diffuse) * material.green + specular,
	               (ambient.blue  + diffuse) * material.blue  + specular};
}

double colorvariance(Color expected, Color samples[], size_t n)
{
	double sum = 0;
	for (size_t i = 0; i < n; i++) {
		sum += pow(expected.red - samples[i].red, 2);
		sum += pow(expected.green - samples[i].green, 2);
		sum += pow(expected.blue- samples[i].blue, 2);
	}
	return sum / (3 * n);
}
