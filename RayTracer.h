#ifndef RayTracer_h

#define RayTracer_h

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector> //Notice that vector in C++ is different from Vector2, Vector3 or similar things in a graphic library.
#include <iostream>
#include <fstream> //Provides facilities for file-based input and output.
#include <cstring>

#include <GLUT/glut.h> //OpenGL Utility Toolkits

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Ray.h"
#include "Object.h"

bool CheckIntersection(const Ray &ray, IntersectInfo &info);
float CastRay(Ray &ray, Payload &payload);

#endif
