.PHONY: run
run: raytracer
	./raytracer

raytracer: raytracer.c
	gcc -o raytracer raytracer.c -lm
