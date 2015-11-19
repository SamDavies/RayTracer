#include "Object.h"

Material::Material():
    ambient(1.0f),
    diffuse(1.0f),
    specular(1.0f),
    glossiness(10.0f),
    reflection(0.0f),
    refraction(0.0f),
    refractiveIndex(0.0f)
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

    if(discriminant < 0) {
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

bool Plane::Intersect(const Ray &ray, IntersectInfo &info) const {
    float angle = glm::dot(ray.direction, normal);
    // this prevents divide by 0 error
    if (angle != 0) {
        float depth = glm::dot((point - ray.origin), normal) / angle;
        // check if the intercection is infront of the camera
        if (depth > 0) {
            info.hitPoint = ray.origin + depth * ray.direction;
            info.normal = normal;
            info.material = MaterialPtr();
            info.time = glm::length(ray.origin - info.hitPoint);
            return true;
        }
        // here the intercection is behind the camera
        return false;
    }
    return false;
}

bool Triangle::Intersect(const Ray &ray, IntersectInfo &info) const {
    glm::vec3 normal = glm::normalize(glm::cross(point2 - point1, point3 - point1));
    // this is the angle between the normal and the ray direction
    float angle = glm::dot(ray.direction, normal);
    // if the Triangle is perpendicular to the view then it does not intersect
    if (angle != 0) {
        float depth = glm::dot((point1 - ray.origin), normal) / angle;
        if (depth > 0) {
            glm::vec3 hitPoint = ray.origin + depth * ray.direction;

            glm::vec3 hit1 =  hitPoint - point1;
            glm::vec3 hit2 =  hitPoint - point2;
            glm::vec3 hit3 =  hitPoint - point3;

            // check if the ray was on the inside of each line of the triangle
            float determinant1 = glm::dot(normal, glm::cross(point2 - point1, hit1));
            float determinant2 = glm::dot(normal, glm::cross(point3 - point2, hit2));
            float determinant3 = glm::dot(normal, glm::cross(point1 - point3, hit3));

            if(determinant1 >= 0 && determinant2 >= 0 && determinant3 >= 0) {
                info.hitPoint = hitPoint;
                info.normal = normal;
                info.material = MaterialPtr();
                info.time = glm::length(ray.origin - info.hitPoint);
                return true;
            }
        }
    }
    return false;
}
