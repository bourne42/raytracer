/**
 * Conrad Verkler cverkler
 * 15-462 Project 4
 *
 * @file sphere.hpp
 * @brief Class defnition for Sphere.
 *
 * @author Kristin Siu (kasiu)
 * @author Eric Butler (edbutler)
 */

#ifndef _462_SCENE_SPHERE_HPP_
#define _462_SCENE_SPHERE_HPP_

#include "scene/scene.hpp"
#include "raytracer/raytracer.hpp" //for access to struct PixelInfo

namespace _462 {

/**
 * A sphere, centered on its position with a certain radius.
 */
class Sphere : public Geometry
{
public:

    real_t radius;
    const Material* material;

    Sphere();
    virtual ~Sphere();
    virtual void render() const;

	// My functions:
	virtual float getIntersection(Vector3 e, Vector3 d, float startInterval, float endInterval, PixelInfo* intersection);
	/*virtual bool loadTex();*/
};

} /* _462 */

#endif /* _462_SCENE_SPHERE_HPP_ */

