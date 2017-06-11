typedef struct {
	uint8_t blue, green, red;
} rgb24;

int writebitmap(FILE *f, Color img[], size_t width, size_t height);
rgb24 color_to_rgb24(Color x);
