.PHONY: run
run: raytracer
	./raytracer && feh output.bmp -Z --force-aliasing

.PHONY: clean
clean:
	rm *.o raytracer

CC = gcc
CFLAGS = -lm -std=c99 -pedantic

raytracer: raytracer.o color.o geometry.o vector.o writebmp.o
	$(CC) $(CFLAGS) -o $@ $^

raytracer.o: raytracer.c color.h geometry.h vector.h writebmp.h
	$(CC) $(CFLAGS) -o $@ -c $<

color.o: color.c color.h
	$(CC) $(CFLAGS) -o $@ -c $<

geometry.o: geometry.c geometry.h color.h vector.h
	$(CC) $(CFLAGS) -o $@ -c $<

vector.o: vector.c vector.h
	$(CC) $(CFLAGS) -o $@ -c $<

writebmp.o: writebmp.c writebmp.h color.h
	$(CC) $(CFLAGS) -o $@ -c $<
