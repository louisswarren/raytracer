.PHONY: run
run: raytracer
	./raytracer && feh output.bmp -Z --force-aliasing

.PHONY: clean
clean:
	rm *.o raytracer

CC = gcc
CFLAGS = -lm -std=c99 -pedantic

raytracer: raytracer.o geometry.o point.o writebmp.o
	$(CC) $(CFLAGS) -o $@ $^

raytracer.o: raytracer.c geometry.h point.h writebmp.h
	$(CC) $(CFLAGS) -o $@ -c $<

geometry.o: geometry.c geometry.h point.h
	$(CC) $(CFLAGS) -o $@ -c $<

point.o: point.c point.h
	$(CC) $(CFLAGS) -o $@ -c $<

writebmp.o: writebmp.c writebmp.h
	$(CC) $(CFLAGS) -o $@ -c $<
