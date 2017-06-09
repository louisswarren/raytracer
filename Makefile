.PHONY: run
run: raytracer
	./raytracer

.PHONY: clean
clean:
	rm *.o raytracer

CC = gcc
CFLAGS = -lm

raytracer: raytracer.o geometry.o point.o
	$(CC) $(CFLAGS) -o $@ $^

raytracer.o: raytracer.c geometry.h point.h
	$(CC) $(CFLAGS) -o $@ -c $<

geometry.o: geometry.c geometry.h point.h
	$(CC) $(CFLAGS) -o $@ -c $<

point.o: point.c point.h
	$(CC) $(CFLAGS) -o $@ -c $<
