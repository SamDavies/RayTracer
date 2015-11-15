#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Material;

class Ray {
  public:
    glm::vec3 origin;
    glm::vec3 direction;

    Ray(const glm::vec3 &origin, const glm::vec3 &direction):
      origin(origin),
      direction(direction)
    {}

    /* Returns the position of the ray at time t */
    glm::vec3 operator() (const float &t) const {
      return origin + direction*t;
    }
};

class IntersectInfo {
  public:

    IntersectInfo():
      time(std::numeric_limits<float>::infinity()),
      hitPoint(0.0f),
      normal(0.0f),
      material(NULL)
    {}
    // It allows you to init variables in another way. Equal to:
    // IntersectInfo(){
    //   time = std::numeric_limits<float>::infinity();
    //   hitPoint = 0.0f;
    //   normal = 0.0f;
    //   material = NULL;
    // }
    
    /* The position of the intersection in 3D coordinates */
    glm::vec3 hitPoint;
    /* The normal vector of the surface at the point of the intersection */
    glm::vec3 normal;
    /* The time along the ray that the intersection occurs */
    float time;
    /* The material of the object that was intersected */
    const Material *material;

    // Reloading "operator =" for class IntersectInfo
    IntersectInfo &operator =(const IntersectInfo &rhs) {
      hitPoint = rhs.hitPoint;
      material = rhs.material;
      normal = rhs.normal;
      time = rhs.time;
      return *this;
    }
};

class Payload {//In this case, it stands for the light particle shot from a certain point.
  public:
    Payload():
      color(0.0f),
      numBounces(0) 
    {}
    
    glm::vec3 color;//  Each time, intersecting with something will change the color of this Payload.
    int numBounces; //  To make the calculation not so expensive, Ray hits more times than a certain number of bounces will not be taken into consideration.
};

