#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "vector.h"
#include "colour.h"
#include "geometry.h"
#include "scenery.h"
#include "writebmp.h"

extern Scenery scene[100];
extern size_t scene_ctr;


static Colour ambient_light = {0.2, 0.2, 0.2};
static Vector light_pos = {0, 50, -50};

static double fog_dist = 500;
static double fog_depth = 1000;
static Colour fog_colour = {0.8, 0.8, 0.8};

static double aa_threshhold = 0.0001;

int in_shadow(Vector pos, Vector light_dir)
{
	Ray shadow_ray = {pos, light_dir};
	Observation shade = scene_observe(shadow_ray);
	Vector light_relative_dir = vec_sub(&light_pos, &pos);
	double dist_to_light = vec_magnitude(&light_relative_dir);
	return (shade.dist > 0 && shade.dist < dist_to_light);
}

Colour trace_reflection(Vector pos, Vector in_dir, Vector normal)
{
	double incidence = vec_dot(&normal, &in_dir);
	Vector n_2i = vec_scaled(&normal, 2 * incidence);
	Vector refl_dir = vec_sub(&in_dir, &n_2i);
	Ray refl_ray = {pos, refl_dir};
	Observation reflected = scene_observe(refl_ray);
	Scenery *obj = reflected.object;
	if (!obj)
		return fog_colour;
	return obj->colourer(reflected.pos, obj->colour, obj->colour_params);
}


Colour apply_fog(Colour col, double dist)
{
	if (dist <= fog_dist)
		return col;
	double t = (dist - fog_dist) / fog_depth;
	return colour_interpolate(&col, &fog_colour, t < 1 ? t : 1);
}

Colour trace(Ray ray)
{
	Observation observed = scene_observe(ray);
	if (observed.dist <= 0)
		return fog_colour;

	Scenery *obj = observed.object;

	void *shape = obj->shape;
	Colour colour = obj->colourer(observed.pos, obj->colour, obj->colour_params);
	Vector objn = obj->normal(shape, &observed.pos);
	Vector light_dir = vec_sub(&light_pos, &observed.pos);
	vec_normalise(&light_dir);

	if (obj->refl > 0) {
		Colour refl_colour = trace_reflection(observed.pos, ray.dir, objn);
		colour = colour_combine(&colour, &refl_colour, obj->refl);
	}

	double diffuse = vec_dot(&light_dir, &objn);
	if (diffuse <= 0 || in_shadow(observed.pos, light_dir)) {
		return apply_fog(colour_phong(&colour, &ambient_light, 0, 0), observed.dist);
	}

	double spec = 0;
	Vector objn_2diff = vec_scaled(&objn, 2 * diffuse);
	Vector specdir = vec_sub(&objn_2diff, &light_dir);
	vec_normalise(&specdir);
	Vector spec_neg_ray_dir = vec_scaled(&ray.dir, -1);
	double specscale = vec_dot(&specdir, &spec_neg_ray_dir);
	if (specscale < 0)
		spec = pow(specscale, 30);
	return apply_fog(colour_phong(&colour, &ambient_light, diffuse, spec), observed.dist);
}


Colour multisample_pixel(int level, Ray ray, double pixel_size)
{
	double subpixel_size = pixel_size / level;
	Colour col = {0, 0, 0};
	for (int i = 0; i < level; i++) {
		for (int j = 0; j < level; j++) {
			Vector subpos = {(i + 0.5 - level / 2) * subpixel_size,
			                 (j + 0.5 - level / 2) * subpixel_size,
			                 0};
			Vector new_dir = vec_add(&ray.dir, &subpos);
			vec_normalise(&new_dir);
			Ray subray = {ray.pos, new_dir};
			Colour sample = trace(subray);
			col.r += sample.r;
			col.g += sample.g;
			col.b += sample.b;
		}
	}
	col.r /= level * level;
	col.g /= level * level;
	col.b /= level * level;
	return col;
}


int pixel_needs_aa(Colour *frame, int h, int w, int width)
{
	Colour p = frame[h * width + w];
	Colour n[4] = {
		frame[(h - 1) * width + w],
		frame[(h + 1) * width + w],
		frame[h * width + w - 1],
		frame[h * width + w + 1],
	};
	return (colour_variance(&p, n, 4) > aa_threshhold);
}

void draw(Colour *frame, Ray view, double focal, int width, int height)
{
	double pixel_size = width > height ? 2.0 / width : 2.0 / height;

	for (int w = 0; w < width; w++) {
		for (int h = 0; h < height; h++) {
			double x = (w + 0.5 - width  / 2) * pixel_size;
			double y = (h + 0.5 - height / 2) * pixel_size;

			Vector pixel_dir = {x, y, focal}; // This ignores view.dir !!!
			Ray ray = {view.pos, vec_normalised(&pixel_dir)};
			frame[h * width + w] = trace(ray);
		}
	}

	if (aa_threshhold < 0)
		return;

	int aa_level = 4;
	for (int w = 1; w < width - 1; w++) {
		for (int h = 1; h < height - 1; h++) {
			if (pixel_needs_aa(frame, h, w, width)) {
				double x = (w + 0.5 - width  / 2) * pixel_size;
				double y = (h + 0.5 - height / 2) * pixel_size;

				Vector pixel_dir = {x, y, focal}; // This ignores view.dir !!!
				Ray ray = {view.pos, vec_normalised(&pixel_dir)};
				frame[h * width + w] = multisample_pixel(4, ray, pixel_size);
			} else {
				frame[h * width + w] = frame[h * width + w];
			}
		}
	}
}

Colour flat_colour(Vector pos, Colour colour, void *_)
{
	return colour;
}

Colour floor_texture(Vector pos, Colour colour, void *params)
{
	double width = *(double *)params;
	Colour colour2 = *(Colour *)&(((double *)params)[1]);
	int odd_x = (((int) floor(pos.x / width)) % 2) != 0;
	int odd_z = (((int) floor(pos.z / width)) % 2) != 0;
	if (odd_x ^ odd_z == 1)
		return colour;
	return colour2;
}

#define add_sphere(C, Q, X, Y, Z, R) scene[scene_ctr++] = \
	(Scenery){&(Sphere){{X, Y, Z}, R}, C, Q, &sphere_intersect, &sphere_normal, &flat_colour, NULL}

#define add_plane(C, Q, X, Y, Z, U1, U2, U3, W1, W2, W3) scene[scene_ctr++] = \
	(Scenery){&(Plane){{X, Y, Z}, {U1, U2, U3}, {W1, W2, W3}}, \
	C, Q, &plane_intersect, &plane_normal, &flat_colour, NULL}

#define add_infinite_plane(C, Q, X, Y, Z, U1, U2, U3, W1, W2, W3) \
	scene[scene_ctr++] = \
	(Scenery){&(Plane){{X, Y, Z}, {U1, U2, U3}, {W1, W2, W3}}, \
	C, Q, &infplane_intersect, &plane_normal, &flat_colour, NULL}

int print_vector(Vector a)
{
	return printf("(%f, %f, %f)\n", a.x, a.y, a.z);
}

int main(void)
{
	Colour colour_RED = {1, 0, 0};
	Colour colour_GREEN = {0, 1, 0};
	Colour colour_BLUE = {0, 0, 1};
	Colour colour_BLACK = {0, 0, 0};
	Colour colour_GREY = {0.4, 0.4, 0.4};
	Colour colour_WHITE = {1, 1, 1};
	Colour wallcolour = {1, 0.8, 0.4};
	Colour floorcolour = {0.3, 0.3, 0.35};


	double p = 0;
	double r = 1;
	double b = sqrt(3) * r;
	double b2 = sqrt(3) * r / 3;
	double h = sqrt(2.0/3) * 2 * r;

	add_sphere(colour_RED,   0.2,    0, 0,     p,    r);
	add_sphere(colour_GREEN, 0.2,   -r, 0, p + b,    r);
	add_sphere(colour_BLUE,  0.2,    r, 0, p + b,    r);
	add_sphere(colour_BLACK,   1,    0, h, p + b2,   r);


	struct floor_params {
		double width;
		Colour colour2;
	};
	struct floor_params floorparams = {0.7, colour_GREY};
	scene[scene_ctr++] = (Scenery){&(Plane){{0, -r, 0}, {0, 0, 1}, {1, 0, 0}}, colour_WHITE, 0, &infplane_intersect, &plane_normal, &floor_texture, &floorparams};


	Ray view = {{0, 0, -6}, {0, 0, 1}};
	size_t width = 480;
	size_t height = 480;

	Colour *frame = malloc(width * height * sizeof(Colour));
	if (!frame) {
		puts("Out of memory");
		return -1;
	}

	draw(frame, view, 2, width, height);

	FILE *f = fopen("output.bmp", "wb");
	writebitmap(f, frame, width, height);
	fclose(f);
	free(frame);
}
