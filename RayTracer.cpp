#include "RayTracer.h"

int windowX = 640;
int windowY = 480;

// Lighting constants
const glm::vec3 lightPosition(-150, 300, 10);
const glm::vec3 lightIntensity(1, 1, 1);
const float specularIntensity = 10.0;
const float EPSILON = 0.01;
const int REFLECTION_LIMIT = 6;

/*
** std::vector is a data format similar with list in most of  script language,
** which allows users to change its size after claiming.
** The difference is that std::vector is based on array rather than list,
** so it is not so effective when you try to insert a new element,
** but faster while calling for values randomly or add elements by order.
*/
std::vector<Object*> objects;

void cleanup() {
	for(unsigned int i = 0; i < objects.size(); ++i){
		if(objects[i]){
			delete objects[i];
		}
	}
}

/*
** TODO: Function for testing intersection against all the objects in the scene
**
** If an object is hit then the IntersectionInfo object should contain
** the information about the intersection. Returns true if any object is hit,
** false otherwise
**
*/
bool CheckIntersection(const Ray &ray, IntersectInfo &info) {
	IntersectInfo closestObjectInfo;
	closestObjectInfo.time = LONG_MAX;
	bool intersects = false;
	// Check for intersection with all objects
	for (int i=0; i<objects.size(); i++) {
		IntersectInfo currentInfo;
		if (objects[i]->Intersect(ray, currentInfo)) {
			// find the object closest to the origin of the ray
			if (currentInfo.time < closestObjectInfo.time) {
				intersects = true;
				closestObjectInfo = currentInfo;
			}
		}
	}
	// save the closest object info
	info = closestObjectInfo;
	return intersects;
}

glm::vec3 GetPhongColor(const Ray &ray, IntersectInfo &info){
	glm::vec3 color;

	glm::vec3 surfaceNorm = info.normal;
	glm::vec3 lightVec = glm::normalize(lightPosition - info.hitPoint);
	glm::vec3 camPos = glm::normalize(ray.origin - info.hitPoint);
	// use max to clamp the cosAlpha above 0
	float cosAlpha = glm::dot(((2.0f * surfaceNorm * glm::dot(lightVec, surfaceNorm)) - lightVec), camPos);
	cosAlpha = fmax(0.0f, cosAlpha);
	float glossMutiplier = pow(cosAlpha, info.material->glossiness);

	glm::vec3 ambient = info.material->ambient;
	glm::vec3 diffuse = info.material->diffuse * glm::dot(lightVec, surfaceNorm);
	glm::vec3 specular = info.material->specular * glossMutiplier;

	// use max to clamp the diffuse above 0
	diffuse.x = fmax(0.0f, diffuse.x);
	diffuse.y = fmax(0.0f, diffuse.y);
	diffuse.z = fmax(0.0f, diffuse.z);

	return lightIntensity * (specular + diffuse + ambient);
}

bool Shadow(const glm::vec3 shadowOrigin) {
	IntersectInfo shadowInfo;
	Ray shadowRayRaw = Ray(shadowOrigin, glm::normalize(lightPosition - shadowOrigin));
	// adjust for floating point inaccuracies
	Ray shadowRay = Ray(shadowRayRaw(EPSILON), glm::normalize(lightPosition - shadowRayRaw(EPSILON)));

	// only look for shadows up unitl the light source
	float lengthToLight = glm::length(lightPosition - shadowOrigin);
    for (int i=0; i<objects.size(); i++) {
        if (objects[i]->Intersect(shadowRay, shadowInfo)) {
            if (shadowInfo.time < lengthToLight) {
                return true;
            }
        }
    }
    return false;
}

/*
** TODO: Recursive ray-casting function. It might be the most important Function in this demo cause it's the one decides the color of pixels.
**
** This function is called for each pixel, and each time a ray is reflected/used
** for shadow testing. The Payload object can be used to record information about
** the ray trace such as the current color and the number of bounces performed.
** This function should return either the time of intersection with an object
** or minus one to indicate no intersection.
*/
//	The function CastRay() will have to deal with light(), shadow() and reflection(). The impement of them would also be important.
float CastRay(Ray &ray, Payload &payload) {
	IntersectInfo info;

	if (CheckIntersection(ray, info)) {
		glm::vec3 surfaceColour;

		// if the object is in shadow then only use ambient light
		if (Shadow(info.hitPoint)) {
			surfaceColour = info.material->ambient;
		} else {
			surfaceColour = GetPhongColor(ray, info);
		}

		// calculate the reflection
		payload.numBounces += 1;
		// initialise to the surface color
		glm::vec3 reflectionColour = surfaceColour;

		glm::vec3 refelectionDirection = ray.direction - 2*(glm::dot(ray.direction, info.normal)) * info.normal;
		Ray reflectionRayRaw = Ray(info.hitPoint, glm::normalize(refelectionDirection));
		// adjust for floating point inaccuracies
		Ray reflectionRay = Ray(reflectionRayRaw(EPSILON), glm::normalize(refelectionDirection));

		if (payload.numBounces < REFLECTION_LIMIT) {
			float reflectionTime = CastRay(reflectionRay, payload);
			reflectionColour = payload.color;
		}

		// merge the reflection and the surface colours
		float reflectivity = info.material->reflection;
		payload.color = (reflectivity * reflectionColour) + ((1-reflectivity) * surfaceColour);
		return info.time;
	}

	// The Ray from camera hits nothing so nothing will be seen. In this case, the pixel should be totally black.
	payload.color = glm::vec3(0.0f, 0.0f, 0.0f);
	return -1.0f;
}


// Render Function

// This is the main render function, it draws pixels onto the display
// using GL_POINTS. It is called every time an update is required.

// This function transforms each pixel into the space of the virtual
// scene and casts a ray from the camera in that direction using CastRay
// And for rendering,
// 1)Clear the screen so we can draw a new frame
// 2)Cast a ray into the scene for each pixel on the screen and use the returned color to render the pixel
// 3)Flush the pipeline so that the instructions we gave are performed.

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);// Clear OpenGL Window

	//	Three parameters of lookat(vec3 eye, vec3 center, vec3 up).
	glm::mat4 viewMatrix = glm::lookAt(glm::vec3(-10.0f,10.0f,10.0f), glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f));
	glm::mat4 projMatrix = glm::perspective(45.0f, (float)windowX / (float)windowY, 1.0f, 10000.0f);

	glBegin(GL_POINTS);	//Using GL_POINTS mode. In this mode, every vertex specified is a point.
	//	Reference https://en.wikibooks.org/wiki/OpenGL_Programming/GLStart/Tut3 if interested.

	for(int x = 0; x < windowX; ++x)
		for(int y = 0; y < windowY; ++y){//Cover the entire display zone pixel by pixel, but without showing.
			float pixelX =  2*((x+0.5f)/windowX)-1;	//Actually, (pixelX, pixelY) are the relative position of the point(x, y).
			float pixelY = -2*((y+0.5f)/windowY)+1;	//The displayzone will be decribed as a 2.0f x 2.0f platform and coordinate origin is the center of the display zone.

			//	Decide the direction of each of the ray.
			glm::vec4 worldNear = glm::inverse(viewMatrix) * glm::inverse(projMatrix) * glm::vec4(pixelX, pixelY, -1, 1);
			glm::vec4 worldFar  = glm::inverse(viewMatrix) * glm::inverse(projMatrix) * glm::vec4(pixelX, pixelY,  1, 1);
			glm::vec3 worldNearPos = glm::vec3(worldNear.x, worldNear.y, worldNear.z) / worldNear.w;
			glm::vec3 worldFarPos  = glm::vec3(worldFar.x, worldFar.y, worldFar.z) / worldFar.w;

			Payload payload;
			Ray ray(worldNearPos, glm::normalize(glm::vec3(worldFarPos - worldNearPos))); //Ray(const glm::vec3 &origin, const glm::vec3 &direction)

			if(CastRay(ray,payload) > 0.0f){
				glColor3f(payload.color.x,payload.color.y,payload.color.z);
			}
			else {
				glColor3f(1,0,0);
			}

			glVertex3f(pixelX,pixelY,0.0f);
		}

	glEnd();
	glFlush();
}

int main(int argc, char **argv) {

  	//initialise OpenGL
	glutInit(&argc, argv);
	//Define the window size with the size specifed at the top of this file
	glutInitWindowSize(windowX, windowY);

	//Create the window for drawing
	glutCreateWindow("RayTracer");
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	//Set the function demoDisplay (defined above) as the function that
	//is called when the window must display.
	glutDisplayFunc(Render);

	// this can be used as a global transform for every object if I'm feeling lazy
	glm::mat4 transform1(0.0f);


	Material chrome = Material(glm::vec3(0.9, 0.9, 0.9), glm::vec3(0.9, 0.9, 0.9), glm::vec3(0.8, 0.8, 1.0), 20, 0.75);
	Material glossGreen = Material(glm::vec3(0.01, 0.05, 0.02), glm::vec3(0.4, 0.6, 0.3), glm::vec3(0.5, 0.5, 0.5), 30, 0.1);
	Material glossRed = Material(glm::vec3(0.05, 0.03, 0.03), glm::vec3(1.0, 0.3, 0.3), glm::vec3(0.7, 0.7, 0.7), 10, 0.2);
	Material shinnyLightBlue = Material(glm::vec3(0.01, 0.05, 0.02), glm::vec3(0.3, 0.3, 1.0), glm::vec3(0.2, 0.2, 0.2), 60, 0.3);
	Material whiteWall = Material(glm::vec3(0.3, 0.3, 0.4), glm::vec3(0.78, 0.78, 0.8), glm::vec3(0.78, 0.78, 0.8), 20, 0.6);
	Material floorGreen = Material(glm::vec3(0.03, 0.03, 0.03), glm::vec3(0.8, 1.0, 0.9), glm::vec3(0.5, 0.5, 0.5), 20, 0.0);

	Sphere sphere1(transform1, chrome, glm::vec3(140, -170, -150), 30.0);
	Sphere sphere2(transform1, glossRed, glm::vec3(140, -180, -90), 20.0);
	Sphere sphere3(transform1, glossGreen, glm::vec3(190, -178, -110), 22.0);
	Sphere sphere4(transform1, shinnyLightBlue, glm::vec3(220, -179, -160), 19.0);

	Plane plane1(transform1, whiteWall, glm::vec3(0, 0, -250), glm::vec3(0, 0, 1));
	Plane plane2(transform1, whiteWall, glm::vec3(250, 0, 0), glm::vec3(-1, 0, 0));
	Plane floorPlane(transform1, whiteWall, glm::vec3(0, -200, 0), glm::vec3(0, 1, 0));
	Plane roofPlane(transform1, whiteWall, glm::vec3(0, 500, 0), glm::vec3(0, -1, 0));

	// use this to push objects into the vector
	objects.push_back(&sphere1);
	objects.push_back(&sphere2);
	objects.push_back(&sphere3);
	objects.push_back(&sphere4);

	objects.push_back(&plane1);
	objects.push_back(&plane2);
	objects.push_back(&floorPlane);
	// objects.push_back(&roofPlane);

	atexit(cleanup);
	glutMainLoop();
}
