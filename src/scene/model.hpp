/**
 * Conrad Verkler cverkler
 * 15-462 Project 4
 *
 * @file model.hpp
 * @brief Model class
 *
 * @author Eric Butler (edbutler)
 */

#ifndef _462_SCENE_MODEL_HPP_
#define _462_SCENE_MODEL_HPP_

#include "scene/scene.hpp"
#include "scene/mesh.hpp"
#include "raytracer/raytracer.hpp" //for access to struct PixelInfo

namespace _462 {

/**
 * A mesh of triangles.
 */
class Model : public Geometry
{
public:

    const Mesh* mesh;
    const Material* material;

    Model();
    virtual ~Model();

    virtual void render() const;

	// My functions:
	virtual float getIntersection(Vector3 e, Vector3 d, float startInterval, float endInterval, PixelInfo* intersection);
	/*virtual bool loadTex();*/
};


} /* _462 */

#endif /* _462_SCENE_MODEL_HPP_ */

