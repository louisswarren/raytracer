#include "color.h"

Color
coloradd(Color a, Color b, double b_scale)
{
	Color r = {a.red + b.red * b_scale,
		       a.green + b.green * b_scale,
			   a.blue + b.blue * b_scale};
	return r;
}

Color
phong(Color material, Color ambient, double diffuse, double specular)
{
	Color col = {(ambient.red + diffuse) * material.red  + specular,
	             (ambient.green + diffuse) * material.green  + specular,
	             (ambient.blue + diffuse) * material.blue  + specular};
	return col;
}
