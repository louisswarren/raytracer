#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define buf_is_lit(B, L) (sizeof(L) == (B).n + 1 && !memcmp((L), (B).p, (B).n))

#define debug(...) fprintf(stderr, __VA_ARGS__)
#define die(...) fprintf(stderr, __VA_ARGS__); exit(1)

enum jtype {
	JSTRING,
	JOBJECT,
	JNUMBER,
	JARRAY,
	JTRUE,
	JFALSE,
	JNULL,
};

struct buffer {
	const char *p;
	size_t n;
};

struct number {
	int is_float;
	union {
		int i;
		double d;
	};
};

struct vec {
	double x, y, z;
};

struct col {
	float r, g, b;
};

static struct {
	struct {
		int width, height;
	} image;
	struct {
		struct vec pos, dir;
	} perspective;
	struct {
		struct col col;
		struct vec pos;
	} lighting;
	struct {
		struct col col;
		double distance, depth;
	} fog;
	struct {
		double threshhold;
	} antialiasing;
} cfg;


size_t
get_strsz(const char *s)
{
	size_t n = 0;
	while (1) {
		switch (s[n]) {
		case '"':
			return n;
		case '\\':
			++n;
			if (s[n] != '"' && s[n] != '\\')
				break;
			/* FALLTHROUGH */
		default:
			++n;
		}
	}
}

void
consume_whitespace(const char **s)
{
	while (**s <= 32) ++(*s);
}

enum jtype
next_type(const char *s)
{
	switch (*s) {
	case '"':
		return JSTRING;
	case '{':
		return JOBJECT;
	case '-':
		return JNUMBER;
	case '[':
		return JARRAY;
	case 't':
		return JTRUE;
	case 'f':
		return JFALSE;
	case 'n':
		return JNULL;
	default:
		if ('0' <= *s && *s <= '9')
			return JNUMBER;
	}
	return -1;
}

/*
const char *
j_string(const char *s, char *buf)
{
	s = j_whitespace(s);
	assert(*s == '"');
	++s;
	size_t sz = get_strsz(s);
	assert(sz > 0);
	memcpy(buf, s, sz - 1);
	buf[sz - 1] = '\0';
	return s + sz;
}
*/

void
start_object(const char **s)
{
	assert(**s == '{');
	(*s)++;
	consume_whitespace(s);
}

struct buffer
consume_string(const char **s)
{
	struct buffer b;
	assert(**s == '"');
	b.p = ++(*s);
	b.n = get_strsz(*s);
	*s += b.n + 1;
	consume_whitespace(s);
	return b;
}

int
try_consume_key(const char **s, struct buffer *key)
{
	if (**s == ',') {
		(*s)++;
		consume_whitespace(s);
	}
	if (**s == '}') {
		++(*s);
		return 0;
	}
	*key = consume_string(s);
	assert(**s == ':');
	++(*s);
	consume_whitespace(s);
	return 1;
}

//struct number
//consume_num(const char **s)
//{
//	struct number num = {0};
//	int i;
//	double d;
//	int i_len, d_len;
//	sscanf(*s, "%d%n", &i, &i_len);
//	sscanf(*s, "%lf%n", &d, &d_len);
//	if (d_len > i_len) {
//		num.d = d;
//		num.is_float = 1;
//		(*s) += d_len;
//	} else {
//		num.i = i;
//		num.is_float = 0;
//		(*s) += i_len;
//	}
//	consume_whitespace(s);
//	return num;
//}

void
consume_double(const char **s, double *x)
{
	int len;
	sscanf(*s, "%lf%n", x, &len);
	*s += len;
	consume_whitespace(s);
}

void
consume_int(const char **s, int *x)
{
	int len;
	sscanf(*s, "%d%n", x, &len);
	*s += len;
	consume_whitespace(s);
}

void
consume_vec(const char **s, struct vec *v)
{
	assert(**s == '[');
	(*s)++;
	consume_whitespace(s);
	consume_double(s, &(v->x));
	assert(**s == ',');
	(*s)++;
	consume_whitespace(s);
	consume_double(s, &(v->y));
	assert(**s == ',');
	(*s)++;
	consume_whitespace(s);
	consume_double(s, &(v->z));
	assert(**s == ']');
	(*s)++;
	consume_whitespace(s);
}

void
consume_col(const char **s, struct col *c)
{
	int rx, gx, bx;
	struct buffer b = consume_string(s);
	sscanf(b.p, "#%02X%02X%02X", &rx, &gx, &bx);
	c->r = rx / 255.0;
	c->g = gx / 255.0;
	c->b = bx / 255.0;
}

void
parsecfg_antialiasing(const char **s)
{
	struct buffer key;
	start_object(s);
	while (try_consume_key(s, &key)) {
		if (buf_is_lit(key, "threshhold")) {
			consume_double(s, &cfg.antialiasing.threshhold);
		} else {
			die("Invalid option antialiasing->%.*s", (int) key.n, key.p);
		}
	}
}

void
parsecfg_fog(const char **s)
{
	struct buffer key;
	start_object(s);
	while (try_consume_key(s, &key)) {
		if (buf_is_lit(key, "colour")) {
			consume_col(s, &cfg.fog.col);
		} else if (buf_is_lit(key, "depth")) {
			consume_double(s, &cfg.fog.depth);
		} else if (buf_is_lit(key, "distance")) {
			consume_double(s, &cfg.fog.distance);
		} else {
			die("Invalid option fog->%.*s", (int) key.n, key.p);
		}
	}
}

void
parsecfg_image(const char **s)
{
	struct buffer key;
	start_object(s);
	while (try_consume_key(s, &key)) {
		if (buf_is_lit(key, "width")) {
			consume_int(s, &cfg.image.width);
		} else if (buf_is_lit(key, "height")) {
			consume_int(s, &cfg.image.height);
		} else {
			die("Invalid option image->%.*s", (int) key.n, key.p);
		}
	}
}

void
parsecfg_lighting(const char **s)
{
	struct buffer key;
	start_object(s);
	while (try_consume_key(s, &key)) {
		if (buf_is_lit(key, "position")) {
			consume_vec(s, &cfg.lighting.pos);
		} else if (buf_is_lit(key, "colour")) {
			consume_col(s, &cfg.lighting.col);
		} else {
			die("Invalid option lighting->%.*s", (int) key.n, key.p);
		}
	}
}

void
parsecfg_perspective(const char **s)
{
	struct buffer key;
	start_object(s);
	while (try_consume_key(s, &key)) {
		if (buf_is_lit(key, "position")) {
			consume_vec(s, &cfg.perspective.pos);
		} else if (buf_is_lit(key, "direction")) {
			consume_vec(s, &cfg.perspective.dir);
		} else {
			die("Invalid option perspective->%.*s", (int) key.n, key.p);
		}
	}
}

int
main(void)
{
	char input[2048];
	const char *s;
	struct buffer key;

	FILE *fp = fopen("scene.json", "r");
	fread(input, 1, sizeof(input), fp);
	s = input;

	start_object(&s);
	while (try_consume_key(&s, &key)) {
		if (buf_is_lit(key, "antialiasing")) {
			parsecfg_antialiasing(&s);
		} else if (buf_is_lit(key, "fog")) {
			parsecfg_fog(&s);
		} else if (buf_is_lit(key, "image")) {
			parsecfg_image(&s);
		} else if (buf_is_lit(key, "lighting")) {
			parsecfg_lighting(&s);
		} else if (buf_is_lit(key, "perspective")) {
			parsecfg_perspective(&s);
		} else {
			die("Invalid option %.*s", (int) key.n, key.p);
		}
	}
}
