#include <errno.h>
#include <error.h>
#include <png.h>

/* These formulae were taken from desert256.vim. Thanks, Henry! */

#define gn(x) (x < 14 ? 0 : (x - 8) / 10 + ((x - 8) % 10 > 4))
#define gl(n) (n ? n * 10 + 8 : 0)
#define gc(n) (n ? 231 + n % 5 * 5 : 16)
#define rn(x) (x < 75 ? 0 : (x - 55) / 40 + ((x - 55) % 40 > 19))
#define rl(n) (n ? n * 40 + 55 : 0)
#define rc(x, y, z) (x * 36 + y * 6 + z + 16)

static int rgb2term(int r, int g, int b)
{
	int gr, gg, gb;

	if ((gr = gn(r)) == (gb = gn(b)) && gb == (gg = gn(g))) {
		int dcr = rl(gr) - r, dcg = rl(gg) - g, dcb = rl(gb) - b;
		int dgr = gl(gr) - r, dgg = gl(gg) - g, dgb = gl(gb) - b;
		int dc = dcr * dcr + dcg * dcg + dcb * dcb;
		int dg = dgr * dgr + dgg * dgg + dgb * dgb;
		if (dg < dc)
			return gc(rn(r));
	}

	return rc(rn(r), rn(g), rn(b));
}

static void paint(png_structp png_ptr, png_infop info_ptr)
{
	int i, j, k, top, bot = 0, altop, albot = 0;
	int w = png_get_image_width(png_ptr, info_ptr);
	int h = png_get_image_height(png_ptr, info_ptr);
	png_bytepp rows = png_get_rows(png_ptr, info_ptr);

	for (i = 0; i < h; i += 2) {
		for (j = k = 0; j < w; k = ++j * 4) {
			top = rgb2term(rows[i][k], rows[i][k + 1], rows[i][k + 2]);
			altop = !rows[i][k + 3];

			if (i == h - 1 && h % 2)
				albot = 1;
			else {
				++i;
				bot = rgb2term(rows[i][k], rows[i][k + 1], rows[i][k + 2]);
				albot = !rows[i--][k + 3];
			}

			if (altop && albot)
				putchar(' ');
			else if (altop + albot)
				printf("\033[38;5;%dm%s\033[m", altop ? bot : top, altop ? "▄" : "▀");
			else if (top == bot)
				printf("\033[48;5;%dm \033[m", top);
			else
				printf("\033[48;5;%d;38;5;%dm▀\033[m", bot, top);
		}
		puts("");
	}
}

int main(int argc, char **argv)
{
	FILE *img;
	char header[4];
	png_structp png_ptr;
	png_infop info_ptr;

	/* Preliminary sanity checks */

	if (argc < 2)
		return puts("Usage: png2term FILE");

	if ((img = fopen(argv[1], "rb")) == NULL)
		error(1, errno, "%s", argv[1]);

	if (fread(header, 1, 4, img) != 4) {
		fclose(img);
		return puts("PNG header not found.");
	}

	if (png_sig_cmp((png_bytep) header, 0, 4)) {
		fclose(img);
		return puts("PNG header not found.");
	}

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fclose(img);
		return puts("Initialization failed.");
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fclose(img);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return puts("Initialization failed.");
	}

	/* All internal errors wind up here, and libpng tends to be very
	   thorough in such cases so a simple exit status is sufficient. */

	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return !fclose(img);
	}

	/* Read, massage, and paint the sprite. */

	png_init_io(png_ptr, img);
	png_set_sig_bytes(png_ptr, 4);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, NULL);
	paint(png_ptr, info_ptr);

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	return fclose(img);
}
