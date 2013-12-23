/**
 * Conrad Verkler cverkler
 * 15-462 Project 4
 *
 * @file model.cpp
 * @brief Model class
 *
 * @author Eric Butler (edbutler)
 * @author Zeyang Li (zeyangl)
 */

#include "scene/model.hpp"
#include "scene/material.hpp"
#include <GL/gl.h>
#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <sstream>

//for cout:
//#include <iostream>
//using namespace std;

namespace _462 {

Model::Model() : mesh( 0 ), material( 0 ) { }
Model::~Model() { }

void Model::render() const
{
    if ( !mesh )
        return;
    if ( material )
        material->set_gl_state();
    mesh->render();
    if ( material )
        material->reset_gl_state();
}

/**
* Intersection with model
*/
float Model::getIntersection(Vector3 eyeOrig, Vector3 delOrig, float startInterval, float endInterval, PixelInfo* intersection)
{
	//transform e and d:
	Vector3 eye=((*inverseTransMat)*getV4(eyeOrig, 1)).xyz();
	Vector3 del=((*inverseTransMat)*getV4(delOrig, 0)).xyz();

	MeshVertex v1, v2, v3;

	float t=endInterval;
	float alpha, beta, gamma, tmp;
	bool foundOne=false;

	for(int i=0; i<mesh->num_triangles(); i++)
	{
		v1=mesh->get_vertices()[mesh->get_triangles()[i].vertices[0]];
		v2=mesh->get_vertices()[mesh->get_triangles()[i].vertices[1]];
		v3=mesh->get_vertices()[mesh->get_triangles()[i].vertices[2]];
		tmp=triangleIntersection(eye, del, startInterval, t, &alpha, &beta, &gamma,
			v1.position, v2.position, v3.position);
		if(tmp)
		{
			if(!intersection)//if no intersection pointer given then just return if the ray hit something
				return tmp;
			//if in this block then tmp<t so it is a close intersection point
			foundOne=true;
			t=tmp;
			// set hit information relevant to point
			intersection->normal = normalize((*normalMat)*((alpha*v1.normal) + (beta*v2.normal) + (gamma*v3.normal)));
			intersection->tex_coord = (alpha*v1.tex_coord) + (beta*v2.tex_coord) + (gamma*v3.tex_coord);
			intersection->intersectionPoint = eyeOrig+(t*delOrig);
		}
	}
	if(!foundOne)
		return 0;

	// set intersection data that is constant for the whole material
	if(intersection)
	{
		intersection->ambient=material->ambient;
		intersection->diffuse=material->diffuse;
		intersection->specular=material->specular;
		intersection->refractive_index=material->refractive_index;
		intersection->tex_color=getTextureColor(intersection->tex_coord, material);
	}

	return t;
}



} /* _462 */

