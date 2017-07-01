#include <math.h>

#include "vector.h"

Vector vec_add(Vector *this, Vector *other)
{
	return (Vector){
		this->x + other->x,
		this->y + other->y,
		this->z + other->z,
	};
}

Vector vec_sub(Vector *this, Vector *other)
{
	return (Vector){
		this->x - other->x,
		this->y - other->y,
		this->z - other->z,
	};
}

void vec_scale(Vector *this, double s)
{
	this->x *= s;
	this->y *= s;
	this->z *= s;
}

Vector vec_scaled(Vector *this, double s)
{
	return (Vector){
		this->x * s,
		this->y * s,
		this->z * s,
	};
}

double vec_dot(Vector *this, Vector *other)
{
	return (this->x * other->x) + (this->y * other->y) + (this->z * other->z);
}

double vec_magnitude(Vector *this)
{
	return sqrt(vec_dot(this, this));
}

void vec_normalise(Vector *this)
{
	vec_scale(this, 1/vec_magnitude(this));
}

Vector vec_normalised(Vector *this)
{
	return vec_scaled(this, 1/vec_magnitude(this));
}

Vector vec_cross(Vector *this, Vector *other)
{
	return (Vector){
		this->y * other->z - this->z * other->y,
		this->z * other->x - this->x * other->z,
		this->x * other->y - this->y * other->x,
	};
}
