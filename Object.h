#pragma once

#include "Ray.h"

class Material {
  public:
    Material();
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float glossiness; //Specular intensity
    float reflection;
    float refraction;
    float refractiveIndex;

    Material(glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float gloss, float refl, float refr, float refrIndex)
      :ambient(amb) ,diffuse(diff) ,specular(spec) ,glossiness(gloss) ,reflection(refl), refraction(refr), refractiveIndex(refrIndex)
      {}
};

// The father class of all the objects displayed. Some features would be shared between objects, others will be overloaded.
class Object {
  public:
    Object(const glm::mat4 &transform = glm::mat4(1.0f), const Material &material = Material());
    //  The keyword const here will check the type of the parameters and make sure no changes are made
    //  to them in the function. It's not necessary but better for robustness
    virtual bool Intersect(const Ray &ray, IntersectInfo &info) const { return true; }
    glm::vec3 Position() const { return glm::vec3(transform[3][0], transform[3][1], transform[3][2]); }

    const Material *MaterialPtr() const { return &material; }
    const Object *ObjectPtr() const { return this; }

    virtual ~Object() {}

  protected:  //  The difference between protected and private is that the protected members will still be available in subclasses.
    glm::mat4 transform;  // Usually a transformation matrix is used to decribe the position from the origin.
    Material material;
};

//  For all those objects added into the scene. Describing them in proper ways and the implement of function Intersect() is what needs to be done.
//  Actually, it's also possible to use some other objects, but those geometries are easy to describe and the intersects are easier to calculate.
//  Try something else if you like, for instance, a box?

class Sphere : public Object {
  glm::vec3 origin;
  float radius;

  public:
    Sphere(const glm::mat4 &transform, const Material &material, glm::vec3 orn, float rad)
      :Object(transform, material)
      ,origin(orn)
      ,radius(rad)
      {}
    virtual bool Intersect(const Ray &ray, IntersectInfo &info) const;
};

class Plane : public Object {
  glm::vec3 point;
  glm::vec3 normal;

  public:
    Plane(const glm::mat4 &transform, const Material &material, glm::vec3 pt, glm::vec3 norm)
      : Object(transform, material)
      , point(pt)
      , normal(glm::normalize(norm))
      {}

    virtual bool Intersect(const Ray &ray, IntersectInfo &info) const;
};

/* TODO: Implement */
class Triangle : public Object {
  public:
    virtual bool Intersect(const Ray &ray, IntersectInfo &info) const;
};
