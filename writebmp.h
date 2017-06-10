typedef struct {
	uint8_t blue, green, red;
} rgb24;

int writebitmap(FILE *f, rgb24 img[], size_t width, size_t height);
