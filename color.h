typedef struct {
	double red, green, blue;
} Color;

Color phong(Color material, Color ambient, double diffuse, double specular);
