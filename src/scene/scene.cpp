/**
 * Conrad Verkler cverkler
 * 15-462 Project 4
 *
 * @file scene.cpp
 * @brief Function definitions for scenes.
 *
 * @author Eric Butler (edbutler)
 * @author Kristin Siu (kasiu)
 */

#include "scene/scene.hpp"
#include "math/math.hpp"

//for cout:
#include <iostream>
using namespace std;

namespace _462 {


Geometry::Geometry():
    position( Vector3::Zero ),
    orientation( Quaternion::Identity ),
    scale( Vector3::Ones )
{
	transMat=new Matrix4;
	inverseTransMat=new Matrix4;
	normalMat=new Matrix3;
}

Geometry::~Geometry()
{
	delete(transMat);
	delete(inverseTransMat);
	delete(normalMat);
}

/** calculates transMat, inverseTransMat, and normalMat by functions in Matrix
*/
void Geometry::defineMI()
{
	make_transformation_matrix(transMat, position, orientation, scale);
	make_inverse_transformation_matrix(inverseTransMat, position, orientation, scale);
	make_normal_matrix(normalMat, *inverseTransMat);
}

Vector4 Geometry::getV4(Vector3 in, int end)
{
	return Vector4(in.x, in.y, in.z, end);
}

float Geometry::triangleIntersection(Vector3 eye, Vector3 del, float startInterval, float endInterval,
	float *alpha, float *beta, float *gamma, Vector3 v1, Vector3 v2, Vector3 v3)
{
	del*=-1; // I have no idea why but without this intersections don't work
	// a-l as defined in shirley book, pg 79
	real_t a=v1.x-v2.x; // x_a-x_b
	real_t b=v1.y-v2.y; // y
	real_t c=v1.z-v2.z; // zw
	real_t d=v1.x-v3.x; // x_a-x_c
	real_t e=v1.y-v3.y; // y
	real_t f=v1.z-v3.z; // z
	real_t g=del.x;
	real_t h=del.y;
	real_t i=del.z;
	real_t j=v1.x-eye.x; // x_a-x_e
	real_t k=v1.y-eye.y; // y
	real_t l=v1.z-eye.z; // z

	real_t eiMhf=(e*i)-(h*f);
	real_t gfMdi=(g*f)-(d*i);
	real_t dhMeg=(d*h)-(e*g);
	real_t akMjb=(a*k)-(j*b);
	real_t jcMal=(j*c)-(a*l);
	real_t blMkc=(b*l)-(k*c);

	real_t M=(a*eiMhf)+(b*gfMdi)+(c*dhMeg);

	float t=((f*akMjb) + (e*jcMal) + (d*blMkc)) / M;
	//t=((a*((e*l)-(f*k)))+(b*((j*k)-(d*i)))+(c*((d*k)-(e*j))))/M;//actual determinant, doesn't need dir to be reversed

	if(t<startInterval || t>endInterval)
		return 0;
	*gamma=((i*akMjb)+(h*jcMal)+(g*blMkc)) / M;
	if((*gamma)<0 || (*gamma)>1)
		return 0;
	*beta=((j*eiMhf)+(k*gfMdi)+(l*dhMeg)) / M;
	if((*beta)<0 || (*beta)>(1-(*gamma)))
		return 0;

	*alpha=1-(*beta)-(*gamma);

	return t;
}

Color3 Geometry::getTextureColor(Vector2 texCoord, const Material *mat)
{
	int width,height;
	mat->get_texture_size(&width, &height);
	if(width<1 || height<1)
		return Color3(1,1,1);
	return mat->get_texture_pixel((int)(texCoord.x*width)%width, (int)(texCoord.y*height)%height);
}


PointLight::PointLight():
    position( Vector3::Zero ),
    color( Color3::White )
{
    attenuation.constant = 1;
    attenuation.linear = 0;
    attenuation.quadratic = 0;
}


Scene::Scene()
{
    reset();
}

Scene::~Scene()
{
    reset();
}

Geometry* const* Scene::get_geometries() const
{
    return geometries.empty() ? NULL : &geometries[0];
}

size_t Scene::num_geometries() const
{
    return geometries.size();
}

const PointLight* Scene::get_lights() const
{
    return point_lights.empty() ? NULL : &point_lights[0];
}

size_t Scene::num_lights() const
{
    return point_lights.size();
}

Material* const* Scene::get_materials() const
{
    return materials.empty() ? NULL : &materials[0];
}

size_t Scene::num_materials() const
{
    return materials.size();
}

Mesh* const* Scene::get_meshes() const
{
    return meshes.empty() ? NULL : &meshes[0];
}

size_t Scene::num_meshes() const
{
    return meshes.size();
}

void Scene::reset()
{
    for ( GeometryList::iterator i = geometries.begin(); i != geometries.end(); ++i ) {
        delete *i;
    }
    for ( MaterialList::iterator i = materials.begin(); i != materials.end(); ++i ) {
        delete *i;
    }
    for ( MeshList::iterator i = meshes.begin(); i != meshes.end(); ++i ) {
        delete *i;
    }

    geometries.clear();
    materials.clear();
    meshes.clear();
    point_lights.clear();

    camera = Camera();

    background_color = Color3::Black;
    ambient_light = Color3::Black;
    refractive_index = 1.0;
}

void Scene::add_geometry( Geometry* g )
{
    geometries.push_back( g );
}

void Scene::add_material( Material* m )
{
    materials.push_back( m );
}

void Scene::add_mesh( Mesh* m )
{
    meshes.push_back( m );
}

void Scene::add_light( const PointLight& l )
{
    point_lights.push_back( l );
}


} /* _462 */

