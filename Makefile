.PHONY: default
default: test

.PHONY: test
test: output.bmp
	sha1sum -c test.sha1

.PHONY: run
run: output.bmp
	feh output.bmp -Z --force-aliasing

.PHONY: clean
clean:
	rm -f *.o raytracer output.bmp

WARNINGS  += -pedantic -pedantic-errors -Wno-overlength-strings
WARNINGS  += -fmax-errors=2
WARNINGS  += -Wall -Wextra -Wdouble-promotion -Wformat=2
WARNINGS  += -Wformat-signedness -Wvla -Wformat-truncation=2 -Wformat-overflow=2
WARNINGS  += -Wnull-dereference -Winit-self -Wuninitialized
WARNINGS  += -Wimplicit-fallthrough=4 -Wstack-protector -Wmissing-include-dirs
WARNINGS  += -Wshift-overflow=2 -Wswitch-default -Wswitch-enum
WARNINGS  += -Wunused-parameter -Wunused-const-variable=2 -Wstrict-overflow=5
WARNINGS  += -Wstringop-overflow=4 -Wstringop-truncation -Walloc-zero -Walloca
WARNINGS  += -Warray-bounds=2 -Wattribute-alias=2 -Wlogical-op
WARNINGS  += -Wduplicated-branches -Wduplicated-cond -Wtrampolines -Wfloat-equal
WARNINGS  += -Wunsafe-loop-optimizations -Wshadow
WARNINGS  += -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion
WARNINGS  += -Wpacked -Wdangling-else -Wno-parentheses -Wsign-conversion
WARNINGS  += -Wdate-time -Wjump-misses-init -Wreturn-local-addr -Wno-pointer-sign
WARNINGS  += -Wstrict-prototypes #-Wold-style-definition
WARNINGS  += -Wmissing-prototypes
WARNINGS  += -Wmissing-declarations -Wnormalized=nfkc -Wredundant-decls
WARNINGS  += -Wnested-externs -Wno-missing-field-initializers -fanalyzer

CC = gcc
CFLAGS = -fopenmp -lm -std=c99 $(WARNINGS) -Werror

output.bmp: raytracer
	./$<

raytracer: raytracer.o colour.o geometry.o scenery.o vector.o writebmp.o
	$(CC) $(CFLAGS) -o $@ $^

raytracer.o: raytracer.c colour.h geometry.h scenery.h vector.h writebmp.h
	$(CC) $(CFLAGS) -o $@ -c $<

colour.o: colour.c colour.h
	$(CC) $(CFLAGS) -o $@ -c $<

geometry.o: geometry.c geometry.h vector.h
	$(CC) $(CFLAGS) -o $@ -c $<

scenery.o: scenery.c scenery.h colour.h vector.h
	$(CC) $(CFLAGS) -o $@ -c $<

vector.o: vector.c vector.h
	$(CC) $(CFLAGS) -o $@ -c $<

writebmp.o: writebmp.c writebmp.h colour.h
	$(CC) $(CFLAGS) -o $@ -c $<
