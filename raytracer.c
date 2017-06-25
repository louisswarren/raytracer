#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "vector.h"
#include "color.h"
#include "geometry.h"
#include "writebmp.h"


typedef double intersect_func(void *, Vector, Vector);
typedef Vector normal_func(void *, Vector);
typedef Color color_func(Vector, Color, void *);

typedef struct {
	void *shape;
	Color color;
	double refl;
	intersect_func *intersect;
	normal_func *normal;
	color_func *colorer;
	void * color_params;
} Scenery;

typedef struct {
	Scenery *object;
	double dist;
	Vector pos;
} Observation;


static Scenery scene[100];
static size_t scene_ctr = 0;

static Color ambient_light = {0.2, 0.2, 0.2};
static Vector light_pos = {0, 50, -50};

static double fog_dist = 500;
static double fog_depth = 1000;
static Color fog_color = {0.8, 0.8, 0.8};

static double aa_threshhold = 0.0001;

Observation observe(Ray ray)
{
	size_t argmin_dist;
	double dist, min_dist = -1;
	for (size_t i = 0; i < scene_ctr; i++) {
		dist = scene[i].intersect(scene[i].shape, ray.pos, ray.dir);
		if (dist > 0 && (min_dist < 0 || dist < min_dist)) {
			min_dist = dist;
			argmin_dist = i;
		}
	}
	if (min_dist < 0)
		return (Observation){NULL, -1, {0, 0, 0}};
	Vector pos = vecadd(ray.pos, vecscale(ray.dir, min_dist));
	return (Observation){&scene[argmin_dist], min_dist, pos};
}

int in_shadow(Vector pos, Vector light_dir)
{
	Ray shadow_ray = {pos, light_dir};
	Observation shade = observe(shadow_ray);
	double dist_to_light = vecmagnitude(vecsub(light_pos, pos));
	return (shade.dist > 0 && shade.dist < dist_to_light);
}

Color trace_reflection(Vector pos, Vector in_dir, Vector normal)
{
	double incidence = vecdot(normal, in_dir);
	Vector refl_dir = vecsub(in_dir, vecscale(normal, 2 * incidence));
	Ray refl_ray = {pos, refl_dir};
	Observation reflected = observe(refl_ray);
	Scenery *obj = reflected.object;
	if (!obj)
		return fog_color;
	return obj->colorer(reflected.pos, obj->color, obj->color_params);
}


Color apply_fog(Color col, double dist)
{
	if (dist <= fog_dist)
		return col;
	double t = (dist - fog_dist) / fog_depth;
	return colorinterpolate(col, fog_color, t < 1 ? t : 1);
}

Color trace(Ray ray)
{
	Observation observed = observe(ray);
	if (observed.dist <= 0)
		return fog_color;

	Scenery *obj = observed.object;

	void *shape = obj->shape;
	Color color = obj->colorer(observed.pos, obj->color, obj->color_params);
	Vector objn = obj->normal(shape, observed.pos);
	Vector light_dir = vecnormalise(vecsub(light_pos, observed.pos));

	if (obj->refl > 0) {
		Color refl_color = trace_reflection(observed.pos, ray.dir, objn);
		color = coloradd(color, refl_color, obj->refl);
	}

	double diffuse = vecdot(light_dir, objn);
	if (diffuse <= 0 || in_shadow(observed.pos, light_dir)) {
		return apply_fog(phong(color, ambient_light, 0, 0), observed.dist);
	}

	double spec = 0;
	Vector specdir = vecsub(vecscale(objn, 2 * diffuse), light_dir);
	double specscale = vecdot(vecnormalise(specdir), vecscale(ray.dir, -1));
	if (specscale < 0)
		spec = pow(specscale, 30);
	return apply_fog(phong(color, ambient_light, diffuse, spec), observed.dist);
}


Color multisample_pixel(int level, Ray ray, double pixel_size)
{
	double subpixel_size = pixel_size / level;
	Color col = {0, 0, 0};
	for (int i = 0; i < level; i++) {
		for (int j = 0; j < level; j++) {
			Vector subpos = {(i + 0.5 - level / 2) * subpixel_size,
			                 (j + 0.5 - level / 2) * subpixel_size,
			                 0};
			Ray subray = {ray.pos, vecnormalise(vecadd(ray.dir, subpos))};
			Color sample = trace(subray);
			col.red += sample.red;
			col.green += sample.green;
			col.blue += sample.blue;
		}
	}
	col.red /= level * level;
	col.green /= level * level;
	col.blue /= level * level;
	return col;
}


int pixel_needs_aa(Color *frame, int h, int w, int width)
{
	Color p = frame[h * width + w];
	Color n[4] = {
		frame[(h - 1) * width + w],
		frame[(h + 1) * width + w],
		frame[h * width + w - 1],
		frame[h * width + w + 1],
	};
	return (colorvariance(p, n, 4) > aa_threshhold);
}

void draw(Color *frame, Ray view, double focal, int width, int height)
{
	double pixel_size = width > height ? 2.0 / width : 2.0 / height;

	for (int w = 0; w < width; w++) {
		for (int h = 0; h < height; h++) {
			double x = (w + 0.5 - width  / 2) * pixel_size;
			double y = (h + 0.5 - height / 2) * pixel_size;

			Vector pixel_dir = {x, y, focal}; // This ignores view.dir !!!
			Ray ray = {view.pos, vecnormalise(pixel_dir)};
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
				Ray ray = {view.pos, vecnormalise(pixel_dir)};
				frame[h * width + w] = multisample_pixel(4, ray, pixel_size);
			} else {
				frame[h * width + w] = frame[h * width + w];
			}
		}
	}
}

Color flat_color(Vector pos, Color color, void *_)
{
	return color;
}

Color floor_texture(Vector pos, Color color, void *params)
{
	double width = *(double *)params;
	Color color2 = *(Color *)&(((double *)params)[1]);
	int odd_x = (((int) floor(pos.x / width)) % 2) != 0;
	int odd_z = (((int) floor(pos.z / width)) % 2) != 0;
	if (odd_x ^ odd_z == 1)
		return color;
	return color2;
}

#define add_sphere(C, Q, X, Y, Z, R) scene[scene_ctr++] = \
	(Scenery){&(Sphere){{X, Y, Z}, R}, C, Q, &intersect_sphere, &normal_sphere, &flat_color, NULL}

#define add_plane(C, Q, X, Y, Z, U1, U2, U3, W1, W2, W3) scene[scene_ctr++] = \
	(Scenery){&(Plane){{X, Y, Z}, {U1, U2, U3}, {W1, W2, W3}}, \
	C, Q, &intersect_plane, &normal_plane, &flat_color, NULL}

#define add_coplane(C, Q, X, Y, Z, U1, U2, U3, W1, W2, W3) scene[scene_ctr++] = \
	(Scenery){&(Plane){{X, Y, Z}, {U1, U2, U3}, {W1, W2, W3}}, \
	C, Q, &intersect_coplane, &normal_plane, &flat_color, NULL}

#define add_infinite_plane(C, Q, X, Y, Z, U1, U2, U3, W1, W2, W3) \
	scene[scene_ctr++] = \
	(Scenery){&(Plane){{X, Y, Z}, {U1, U2, U3}, {W1, W2, W3}}, \
	C, Q, &intersect_infinite_plane, &normal_plane, &flat_color, NULL}

int print_vector(Vector a)
{
	return printf("(%f, %f, %f)\n", a.x, a.y, a.z);
}

int main(void)
{
	Color COLOR_RED = {1, 0, 0};
	Color COLOR_GREEN = {0, 1, 0};
	Color COLOR_BLUE = {0, 0, 1};
	Color COLOR_BLACK = {0, 0, 0};
	Color COLOR_GREY = {0.4, 0.4, 0.4};
	Color COLOR_WHITE = {1, 1, 1};
	Color wallcolor = {1, 0.8, 0.4};
	Color floorcolor = {0.3, 0.3, 0.35};


	double p = 0;
	double r = 1;
	double b = sqrt(3) * r;
	double b2 = sqrt(3) * r / 3;
	double h = sqrt(2.0/3) * 2 * r;

	add_sphere(COLOR_RED,   0.2,    0, 0,     p,    r);
	add_sphere(COLOR_GREEN, 0.2,   -r, 0, p + b,    r);
	add_sphere(COLOR_BLUE,  0.2,    r, 0, p + b,    r);
	add_sphere(COLOR_BLACK,   1,    0, h, p + b2,   r);


	struct floor_params {
		double width;
		Color color2;
	};
	struct floor_params floorparams = {0.7, COLOR_GREY};
	scene[scene_ctr++] = (Scenery){&(Plane){{0, -r, 0}, {0, 0, 1}, {1, 0, 0}}, COLOR_WHITE, 0, &intersect_infinite_plane, &normal_plane, &floor_texture, &floorparams};


	Ray view = {{0, 0, -6}, {0, 0, 1}};
	size_t width = 480;
	size_t height = 480;

	Color *frame = malloc(width * height * sizeof(Color));
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
