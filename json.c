#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define buf_is_lit(B, L) (sizeof(L) == (B).n + 1 && !memcmp((L), (B).p, (B).n))

#define debug(...) fprintf(stderr, __VA_ARGS__)

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
	debug("Started object\n");
	(*s)++;
	consume_whitespace(s);
}

struct buffer
consume_string(const char **s)
{
	struct buffer b;
	assert(**s == '"');
	debug("Started string\n");
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
	if (**s == '}')
		return 0;
	*key = consume_string(s);
	assert(**s == ':');
	++(*s);
	debug("Key consumed\n");
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

int
main(void)
{
	char input[2048];
	const char *s;
	struct buffer key;
	int width, height;

	FILE *fp = fopen("scene.json", "r");
	fread(input, 1, sizeof(input), fp);
	s = input;
	start_object(&s);
	while (try_consume_key(&s, &key)) {
		if (buf_is_lit(key, "image")) {
			debug("Got key image\n");
			start_object(&s);
			while (try_consume_key(&s, &key)) {
				if (buf_is_lit(key, "width")) {
					consume_int(&s, &width);
					printf("Width: %d\n", width);
				} else if (buf_is_lit(key, "height")) {
					consume_int(&s, &height);
					printf("Height: %d\n", height);
				}
			}
			break;
		}
	}
	//printf("key: %s\n", buf);
	//s++;
	//s = j_whitespace(s);
	//s = j_string(s, buf);
	//printf("value: %s\n", buf);
}
