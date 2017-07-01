.PHONY: default
default: run

.PHONY: run
run: raytracer
	./raytracer && feh output.bmp -Z --force-aliasing

.PHONY: clean
clean:
	rm *.o raytracer

CC = gcc
CFLAGS = -lm -std=c99 -pedantic

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
