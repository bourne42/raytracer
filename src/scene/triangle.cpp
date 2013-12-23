/**
 * Conrad Verkler cverkler
 * 15-462 Project 4
 *
 * @file triangle.cpp
 * @brief Function definitions for the Triangle class.
 *
 * @author Eric Butler (edbutler)
 */

#include "scene/triangle.hpp"
#include "application/opengl.hpp"

//for cout:
//#include <iostream>
//using namespace std;

namespace _462 {

Triangle::Triangle()
{
    vertices[0].material = 0;
    vertices[1].material = 0;
    vertices[2].material = 0;
}

Triangle::~Triangle() { }

void Triangle::render() const
{
    bool materials_nonnull = true;
    for ( int i = 0; i < 3; ++i )
        materials_nonnull = materials_nonnull && vertices[i].material;

    // this doesn't interpolate materials. Ah well.
    if ( materials_nonnull )
        vertices[0].material->set_gl_state();

    glBegin(GL_TRIANGLES);

    glNormal3dv( &vertices[0].normal.x );
    glTexCoord2dv( &vertices[0].tex_coord.x );
    glVertex3dv( &vertices[0].position.x );

    glNormal3dv( &vertices[1].normal.x );
    glTexCoord2dv( &vertices[1].tex_coord.x );
    glVertex3dv( &vertices[1].position.x);

    glNormal3dv( &vertices[2].normal.x );
    glTexCoord2dv( &vertices[2].tex_coord.x );
    glVertex3dv( &vertices[2].position.x);

    glEnd();

    if ( materials_nonnull )
        vertices[0].material->reset_gl_state();
}

/**
* Triangle Intersection
*/
float Triangle::getIntersection(Vector3 eyeOrig, Vector3 delOrig, float startInterval, float endInterval, PixelInfo* intersection)
{
	//transform e and d:
	Vector3 eye=((*inverseTransMat)*getV4(eyeOrig, 1)).xyz();
	Vector3 del=((*inverseTransMat)*getV4(delOrig, 0)).xyz();

	float alpha, beta, gamma;
	float t=triangleIntersection(eye, del, startInterval, endInterval, &alpha, &beta, &gamma,
		vertices[0].position, vertices[1].position, vertices[2].position);

	if(!t)
		return 0;

	if(intersection)
	{
		intersection->ambient=(alpha*(vertices[0].material->ambient)) +
			(beta*(vertices[1].material->ambient)) + (gamma*(vertices[2].material->ambient));
		intersection->diffuse=(alpha*(vertices[0].material->diffuse)) +
			(beta*(vertices[1].material->diffuse)) + (gamma*(vertices[2].material->diffuse));
		intersection->specular=(alpha*(vertices[0].material->specular)) +
			(beta*(vertices[1].material->specular)) + (gamma*(vertices[2].material->specular));
		intersection->refractive_index=(alpha*(vertices[0].material->refractive_index)) +
			(beta*(vertices[1].material->refractive_index)) + (gamma*(vertices[2].material->refractive_index));
		intersection->intersectionPoint=eyeOrig+(t*delOrig);
		intersection->normal=normalize((*normalMat)*(alpha*vertices[0].normal + beta*vertices[1].normal + gamma*vertices[2].normal));

		// calculate texture color:
		Vector2 tex_coord=(alpha*vertices[0].tex_coord) + (beta*vertices[1].tex_coord) + (gamma*vertices[2].tex_coord);
		intersection->tex_coord=tex_coord;
		intersection->tex_color=alpha*getTextureColor(tex_coord, vertices[0].material)+
			beta*getTextureColor(tex_coord, vertices[1].material)+
			gamma*getTextureColor(tex_coord, vertices[2].material);//*/
	}

	return t;
}

} /* _462 */

