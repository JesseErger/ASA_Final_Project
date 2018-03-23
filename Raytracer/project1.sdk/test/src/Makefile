debug: *.c
	gcc *.c -o ../raytracer -I. -lm -pg -O1

release: *.c
	gcc *.c -o ../raytracer -I. -lm -O3

clean:
	$(rm) raytracer
