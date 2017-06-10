#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "writebmp.h"

int writefileheader(FILE *f, size_t fsize)
{
	char     bfType[2]   = {'B', 'M'};
	uint32_t bfSize      = fsize;
	uint16_t bfReserved1 = 0;
	uint16_t bfReserved2 = 0;
	uint32_t bfOffBits   = 54;
	fwrite(bfType,       1, 2, f);
	fwrite(&bfSize,      4, 1, f);
	fwrite(&bfReserved1, 2, 1, f);
	fwrite(&bfReserved2, 2, 1, f);
	fwrite(&bfOffBits,   4, 1, f);
	return 14;
}

int writeinfoheader(FILE *f, size_t width, size_t height)
{
	uint32_t biSize          = 40;
	uint32_t biWidth         = width;
	uint32_t biHeight        = height;
	uint16_t biPlanes        = 0;
	uint16_t biBitCount      = 24;
	uint32_t biCompression   = 0;
	uint32_t biSizeImage     = 0;
	uint32_t biXPelsPerMeter = 0;
	uint32_t biYPelsPerMeter = 0;
	uint32_t biClrUsed       = 0;
	uint32_t biClrImportant  = 0;
	fwrite(&biSize,          4, 1, f);
	fwrite(&biWidth,         4, 1, f);
	fwrite(&biHeight,        4, 1, f);
	fwrite(&biPlanes,        2, 1, f);
	fwrite(&biBitCount,      2, 1, f);
	fwrite(&biCompression,   4, 1, f);
	fwrite(&biSizeImage,     4, 1, f);
	fwrite(&biXPelsPerMeter, 4, 1, f);
	fwrite(&biYPelsPerMeter, 4, 1, f);
	fwrite(&biClrUsed,       4, 1, f);
	fwrite(&biClrImportant,  4, 1, f);
	return 40;
}

int writebitmap(FILE *f, rgb24 img[], size_t width, size_t height)
{
	uint32_t fsize = 14 + 40 + width * height * 3;
	char pad[4] = {0, 0, 0, 0};

	writefileheader(f, fsize);
	writeinfoheader(f, width, height);


	size_t pad_len = (width % 4) ? 4 - (width % 4) : 0;
	for (int r = height - 1; r >= 0; r--) {
		fwrite(img + width * r, 3, width, f);
		if (pad_len)
			fwrite(pad, 1, pad_len, f);
	}
	return 0;
}
