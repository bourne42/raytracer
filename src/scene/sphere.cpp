/**
 * Conrad Verkler cverkler
 * 15-462 Project 4
 *
 * @file sphere.cpp
 * @brief Function defnitions for the Sphere class.
 *
 * @author Kristin Siu (kasiu)
 * @author Eric Butler (edbutler)
 */

#include "scene/sphere.hpp"
#include "application/opengl.hpp"

#include "math/math.hpp";

//for cout:
#include <iostream>
using namespace std;

namespace _462 {

#define SPHERE_NUM_LAT 80
#define SPHERE_NUM_LON 100

#define SPHERE_NUM_VERTICES ( ( SPHERE_NUM_LAT + 1 ) * ( SPHERE_NUM_LON + 1 ) )
#define SPHERE_NUM_INDICES ( 6 * SPHERE_NUM_LAT * SPHERE_NUM_LON )
// index of the x,y sphere where x is lat and y is lon
#define SINDEX(x,y) ((x) * (SPHERE_NUM_LON + 1) + (y))
#define VERTEX_SIZE 8
#define TCOORD_OFFSET 0
#define NORMAL_OFFSET 2
#define VERTEX_OFFSET 5

static unsigned int Indices[SPHERE_NUM_INDICES];
static float Vertices[VERTEX_SIZE * SPHERE_NUM_VERTICES];

static void init_sphere()
{
    static bool initialized = false;
    if ( initialized )
        return;

    for ( int i = 0; i <= SPHERE_NUM_LAT; i++ ) {
        for ( int j = 0; j <= SPHERE_NUM_LON; j++ ) {
            real_t lat = real_t( i ) / SPHERE_NUM_LAT;
            real_t lon = real_t( j ) / SPHERE_NUM_LON;
            float* vptr = &Vertices[VERTEX_SIZE * SINDEX(i,j)];

            vptr[TCOORD_OFFSET + 0] = lon;
            vptr[TCOORD_OFFSET + 1] = 1-lat;

            lat *= PI;
            lon *= 2 * PI;
            real_t sinlat = sin( lat );

            vptr[NORMAL_OFFSET + 0] = vptr[VERTEX_OFFSET + 0] = sinlat * sin( lon );
            vptr[NORMAL_OFFSET + 1] = vptr[VERTEX_OFFSET + 1] = cos( lat ),
            vptr[NORMAL_OFFSET + 2] = vptr[VERTEX_OFFSET + 2] = sinlat * cos( lon );
        }
    }

    for ( int i = 0; i < SPHERE_NUM_LAT; i++ ) {
        for ( int j = 0; j < SPHERE_NUM_LON; j++ ) {
            unsigned int* iptr = &Indices[6 * ( SPHERE_NUM_LON * i + j )];

            unsigned int i00 = SINDEX(i,  j  );
            unsigned int i10 = SINDEX(i+1,j  );
            unsigned int i11 = SINDEX(i+1,j+1);
            unsigned int i01 = SINDEX(i,  j+1);

            iptr[0] = i00;
            iptr[1] = i10;
            iptr[2] = i11;
            iptr[3] = i11;
            iptr[4] = i01;
            iptr[5] = i00;
        }
    }

    initialized = true;
}

Sphere::Sphere()
    : radius(0), material(0) {}

Sphere::~Sphere() {}

void Sphere::render() const
{
    // create geometry if we haven't already
    init_sphere();

    if ( material )
        material->set_gl_state();

    // just scale by radius and draw unit sphere
    glPushMatrix();
    glScaled( radius, radius, radius );
    glInterleavedArrays( GL_T2F_N3F_V3F, VERTEX_SIZE * sizeof Vertices[0], Vertices );
    glDrawElements( GL_TRIANGLES, SPHERE_NUM_INDICES, GL_UNSIGNED_INT, Indices );
    glPopMatrix();

    if ( material )
        material->reset_gl_state();
}

/**
* Intersection with sphere
* If intersects twice only return the smaller t
*/
float Sphere::getIntersection(Vector3 eOrig, Vector3 dOrig, float startInterval, float endInterval, PixelInfo* intersection)
{
	//transform e and d:
	Vector3 e=((*inverseTransMat)*getV4(eOrig, 1)).xyz();
	Vector3 d=((*inverseTransMat)*getV4(dOrig, 0)).xyz();

	real_t dDot = dot(d, d);
	Vector3 eMinusC = e-((*inverseTransMat)*getV4(position, 1)).xyz();
	float radical= pow(dot(d, eMinusC), 2) - (dDot*(dot(eMinusC,eMinusC)-pow(radius,2)));
	if(radical<0)//if no intersection
		return 0;
	radical=sqrt(radical);
	float a=(dot((d*-1),eMinusC)-radical)/dDot;
	float b=(dot((d*-1),eMinusC)+radical)/dDot;
	//check out of bounds intersections:
	if((a<startInterval || a>endInterval) && (b<startInterval || b>endInterval))
		return 0;
	if(a<startInterval || a>endInterval)
		return b;
	if(b<startInterval || b>endInterval)
		return a;

	float t = a;//return min of a,b
	if(b<a)
		t=b;

	if(intersection)
	{
		intersection->ambient=material->ambient;
		intersection->diffuse=material->diffuse;
		intersection->specular=material->specular;
		intersection->refractive_index=material->refractive_index;
		intersection->intersectionPoint=eOrig + (t*dOrig);
		intersection->normal=normalize((*normalMat)*(intersection->intersectionPoint-position));

		// this math from cse.msu.edu/~cse872/tutorial4.html
		float tx = atan2(intersection->normal.x, intersection->normal.z) / (2. * PI) + 0.5;
		float ty = asin(intersection->normal.y) / PI + .5;
		intersection->tex_coord=Vector2(tx, ty);
		intersection->tex_color=getTextureColor(intersection->tex_coord, material);
	}

	return t;
}


} /* _462 */

