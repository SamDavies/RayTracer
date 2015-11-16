#include "Object.h"

Material::Material():
    ambient(1.0f),
    diffuse(1.0f),
    specular(1.0f),
    glossiness(10.0f),
    reflection(0.0f)
  {}

Object::Object(const glm::mat4 &transform, const Material &material):
    transform(transform),
    material(material)
  {}

/*
 *
 */
bool Sphere::Intersect(const Ray &ray, IntersectInfo &info) const {
  // solve the quadratic equation
  float a = glm::dot(ray.direction, ray.direction);
  float b = glm::dot(2.0f*ray.direction, (ray.origin - origin));
  float c = glm::dot((ray.origin - origin), (ray.origin - origin)) - pow(radius, 2);
  float discriminant = pow(b, 2) - 4*a*c;

  if(discriminant < 0){
    // here the ray did not intersect with the object
    return false;
  } else if (discriminant >= 0){
    // here the ray passes through 2 surfaces of the sphere, or touches the edge
    // we want to use the closest intersection point and since the sqrt is always positive,
    // we can just use the negative version of the quadratic solution function.
    float depth = (-b -sqrt(discriminant)) / (2.0f*a);
    // ignore intersection if it is behind the camera
    // we make the assumption that if we are on the inside of the object then it is not displayed
    if(depth < 0){
      return false;
    }
    info.hitPoint = ray.origin + (depth * ray.direction);
  }

  info.material = MaterialPtr();
  // calculate the normal on the sphere where the ray intersects it
  info.normal = glm::normalize(info.hitPoint - origin);
  info.time = glm::length(ray.origin - info.hitPoint);

  return true;
}
// Function glm::dot(x,y) will return the dot product of parameters. (It's the inner product of vectors)

/* TODO: Implement */
bool Plane::Intersect(const Ray &ray, IntersectInfo &info) const { return -1.0f; }

/* TODO: Implement */
bool Triangle::Intersect(const Ray &ray, IntersectInfo &info) const { return -1.0f; }
