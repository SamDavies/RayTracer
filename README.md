# Ray Tracer
This is a ray tracer for the CG course at Edinburgh university
The techniques supported are phong lighting, shadows, reflections and refractions.
The objects supported are spheres planes and triangles.

To start the ray tracer, open the root directory and run this command in the terminal:
```
make run
```

## Ray Tracing Intersections
For each pixel in the image, a ray is projected through that pixel. The colour of the pixel is determined by the colour of the point on the first object that it hits in the scene. If no objects are intercepted then the background colour is used.

## Lighting
Exactly 1 light source is placed in the scene.

## Phong Illumination
Phong illumination is used to calculate the base colour of an object and therefore the pixel.

## Shadows
To determine if a pixel is in shadow, a ray is projected from the point of intersection towards the light source. If an object is in the way of this ray then the object is in shadow and only the ambient light is used.

## Reflections
The reflections are calculated by recursively casting a ray along the reflection vector. Only a portion of this ray will be mixed will the final colour of this pixel, depending on the reflection parameter of each material. The limit for the number of recursive reflections is 6 so that infinite loops.

## Refractions
The Fresnel equation equation calculates the refraction using the refractive index of the material it has hit. I assume that the refractive index of the air is 1. This is then mixed with the reflection and the surface colour depending on the refraction parameter of that material.
