typedef struct {
	double red, green, blue;
} Color;

Color coloradd(Color a, Color b, double b_scale);

Color phong(Color material, Color ambient, double diffuse, double specular);

double colorvariance(Color expected, Color samples[], size_t n);
