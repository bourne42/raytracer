/**
 * Conrad Verkler cverkler
 * 15-462 Project 4
 *
 * @file raytacer.hpp
 * @brief Raytracer class
 *
 * Implement these functions for project 2.
 *
 * @author H. Q. Bovik (hqbovik)
 * @bug Unimplemented
 */

#ifndef _462_RAYTRACER_HPP_
#define _462_RAYTRACER_HPP_

#include "math/color.hpp"
#include "math/vector.hpp"
#include "math/quaternion.hpp"
#include <vector>

namespace _462 {

class Scene;

struct PixelInfo
{
	Vector3 intersectionPoint;
	Vector3 normal;
	Vector2 tex_coord;

	Color3 ambient;
	Color3 diffuse;
	Color3 specular;
	Color3 tex_color;
	real_t refractive_index;
};

class Raytracer
{
public:

    Raytracer();

    ~Raytracer();

    bool initialize( Scene* scene, size_t width, size_t height );

    bool raytrace( unsigned char* buffer, real_t* max_time );

private:

    // the scene to trace
    Scene* scene;

    // the dimensions of the image to trace
    size_t width, height;

    // the next row to raytrace
    size_t current_row;

	// use to pass object intersection information
	// only every being used by one ray at a time, so ok to put here
	PixelInfo* intersectionPoint;
};

} /* _462 */

#endif /* _462_RAYTRACER_HPP_ */

