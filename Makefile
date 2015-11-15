CC=g++
# LIBS= -lGL -lglut
LIBS= -framework GLUT -framework OpenGL

all:
	g++ *.cpp $(LIBS) -o RayTracer

run: all
	./RayTracer

clean:
	rm -f RayTracer *.o
