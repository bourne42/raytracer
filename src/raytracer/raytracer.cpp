/**
 * Conrad Verkler cverkler
 * 15-462 Project 4
 *
 * @file raytacer.cpp
 * @brief Raytracer class
 *
 * Implement these functions for project 2.
 *
 * @author H. Q. Bovik (hqbovik)
 * @bug Unimplemented
 */

#include "raytracer.hpp"
#include "scene/scene.hpp"

#include <SDL/SDL_timer.h>
#include <iostream>
#include <float.h>

//for cout:
#include <iostream>
using namespace std;

//define FASTER if render 1/4 of pixels (faster rendering for testin)
//#define FASTER


namespace _462 {

Raytracer::Raytracer()
    : scene( 0 ), width( 0 ), height( 0 ) { }

Raytracer::~Raytracer() { }

/**
 * Initializes the raytracer for the given scene. Overrides any previous
 * initializations. May be invoked before a previous raytrace completes.
 * @param scene The scene to raytrace.
 * @param width The width of the image being raytraced.
 * @param height The height of the image being raytraced.
 * @return true on success, false on error. The raytrace will abort if
 *  false is returned.
 */
bool Raytracer::initialize( Scene* scene, size_t width, size_t height )
{
    this->scene = scene;
    this->width = width;
    this->height = height;

    current_row = 0;

    // TODO any initialization or precompuation before the trace

	//define all MI's
	for(int i=0; i<scene->num_geometries(); i++)
		(scene->get_geometries()[i])->defineMI();

    return true;
}

//if level is greater than this then terminate
const int recursionDepth=3;
const float epsilon=.001;

/**
* recursive computation for computing the color for the ray eye+t*dir in given scene
* Use hitPoint to pass data from intersection functions, so don't have to allocate everytime.
* ip=intersectionPoint
*
* Pass in t for end interval because want to find an object that is closer than the one we already found
* If the intersection point is farther away then the previous intersection information will not be changed.
*
* Only one level of refraction, set to 0 if just in air
*/
Color3 getColor(Vector3 eye, Vector3 dir, float startInterval, float endInterval,
	PixelInfo* ip, const Scene* scene, int recursionLevel, float currentRefraction)
{
	float t=endInterval;
	for(int i=0; i<scene->num_geometries(); i++){
		Geometry* tmp = scene->get_geometries()[i];
		float a=tmp->getIntersection(eye, dir, startInterval, t, ip);
		if(a)
			t=a;
	}

	//if hit an object:
	if(t<endInterval)
	{
		//Refraction:
		if(ip->refractive_index)
		{
			Vector3 normalDir = normalize(dir);
			Vector3 normal = ip->normal;
			float dnDot = dot(normalDir, normal);

			float n;
			float nt;

			if(dnDot>=0)//leaving dieletric
			{
				n=currentRefraction;
				nt=scene->refractive_index;
			}
			else //entering dielectric
			{
				n=scene->refractive_index;
				if(currentRefraction)//transfering to new dieletric
					n=currentRefraction;
				nt=ip->refractive_index;
			}

			//define reflectedColor
			Vector3 newDir = dir - (2*dot(dir, ip->normal)*(ip->normal));
			Vector3 position = ip->intersectionPoint;
			Color3 specCol=ip->specular;
			Color3 reflectedColor=Color3(0,0,0);
			if(ip->specular!=Color3(0,0,0) && recursionLevel<recursionDepth)
					reflectedColor = getColor(position, newDir, epsilon, FLT_MAX, ip, scene,
						recursionLevel+1, n) * specCol;

			float radical=1-((pow(n,2)*(1-pow(dnDot,2)))/pow(nt,2));

			if(radical<0) // total internal reflection
				return reflectedColor;
			else
			{
				float c = -dnDot;
				float R0 = pow(n-1, 2)/pow(n+1,2);
				float R = R0 + ((1-R0)*pow(1-c,5));
				if(R>=1)
					return reflectedColor;
				Vector3 tVec=((n/nt)*(normalDir-(normal*dnDot))) -
					(normal*sqrt(radical));
				if(recursionLevel>=recursionDepth)//if gone too muchjust stop
					return Color3(0,0,0);
				return R*reflectedColor + (1-R)*getColor(position, tVec, epsilon, FLT_MAX, ip, scene,
						recursionLevel+1, nt);
			}
		}//*/

		Color3 finalColor=(scene->ambient_light)*(ip->ambient);

		//adds up specular and diffuse colors for each light
		for(int l=0; l<scene->num_lights(); l++)
		{
			bool shadow=false;
			PointLight light = scene->get_lights()[l];
			Vector3 dirToLight=light.position-(ip->intersectionPoint);
			float lightD=length(dirToLight);

			//check for shadows:
			for(int i=0; i<scene->num_geometries(); i++){
				Geometry* tmp = scene->get_geometries()[i];
				if(tmp->getIntersection(ip->intersectionPoint, dirToLight,
					epsilon, 1, NULL)) //use .01 so it doesn't intersect with itself, arbitrary
				{
					shadow=true;
					break;
				}
			}

			if(!shadow)
			{
				Vector3 lVec=normalize(dirToLight);
				//add diffuse color:
				float dotStuff=dot(ip->normal, lVec);
				if(dotStuff<0)
					dotStuff=0;
				Color3 lightColor=(light.color)*(1/(
					light.attenuation.constant + (lightD*light.attenuation.linear) +
					(pow(lightD, 2)*light.attenuation.quadratic)));
				finalColor+=(ip->diffuse) * lightColor * dotStuff;
			}
		}

		// Recursive call: reflection if specular!=0
		Vector3 newDir = dir - (2*dot(dir, ip->normal)*(ip->normal));
		Color3 texColor=ip->tex_color;
		Color3 specCol=ip->specular;
		if(ip->specular!=Color3(0,0,0) && recursionLevel<recursionDepth && (!currentRefraction))
			finalColor+=getColor(ip->intersectionPoint, newDir, epsilon, FLT_MAX, ip, scene, recursionLevel+1,
				currentRefraction) * specCol;

		finalColor*=texColor;

		return finalColor;
	}

	//no object, return background color:
	return scene->background_color;
}

/**
 * Performs a raytrace on the given pixel on the current scene.
 * The pixel is relative to the bottom-left corner of the image.
 * @param scene The scene to trace.
 * @param x The x-coordinate of the pixel to trace.
 * @param y The y-coordinate of the pixel to trace.
 * @param width The width of the screen in pixels.
 * @param height The height of the screen in pixels.
 * @param intersectionPoint Gives allocated area for intersection information, so don't have to keep allocating
 * @return The color of that pixel in the final image.
 */
static Color3 trace_pixel( const Scene* scene, size_t x, size_t y, size_t width, size_t height, PixelInfo* intersectionPoint)
{
    assert( 0 <= x && x < width );
    assert( 0 <= y && y < height );

	Vector3 eye = scene->camera.get_position(); // location
	Vector3 viewDirection = normalize(scene->camera.get_direction());
	Vector3 up = normalize(scene->camera.get_up());
	// rightImagePlane: defines the vector pointing to the right parallel to the image plane
	Vector3 rightImagePlane =  normalize(cross(viewDirection, up));

	real_t nearClip = scene->camera.get_near_clip();
	real_t fov = scene->camera.get_fov_radians();
	float widthTrans = 2*nearClip*tan(fov/2); // width of image in world coord system
	float heightTrans = widthTrans*height/width; // height of image in world coord system

	//define d, in ray=eye+t*d:
	Vector3 d = Vector3(0,0,0);
	d += viewDirection * nearClip;
	d += up * (((y+.5)*heightTrans/height)-(heightTrans/2));
	d += rightImagePlane * (((x+.5)*widthTrans/width)-(widthTrans/2));

	return getColor(eye, d, 1, FLT_MAX, intersectionPoint, scene, 1, 0);
}

/**
 * Raytraces some portion of the scene. Should raytrace for about
 * max_time duration and then return, even if the raytrace is not copmlete.
 * The results should be placed in the given buffer.
 * @param buffer The buffer into which to place the color data. It is
 *  32-bit RGBA (4 bytes per pixel), in row-major order.
 * @param max_time, If non-null, the maximum suggested time this
 *  function raytrace before returning, in seconds. If null, the raytrace
 *  should run to completion.
 * @return true if the raytrace is complete, false if there is more
 *  work to be done.
 */
bool Raytracer::raytrace( unsigned char *buffer, real_t* max_time )
{
    static const size_t PRINT_INTERVAL = 64;

    // the time in milliseconds that we should stop
    unsigned int end_time = 0;
    bool is_done;

	PixelInfo* intersectionPoint=new PixelInfo;

    if ( max_time ) {
        // convert duration to milliseconds
        unsigned int duration = (unsigned int) ( *max_time * 1000 );
        end_time = SDL_GetTicks() + duration;
    }

    // until time is up, run the raytrace. we render an entire row at once
    // for simplicity and efficiency.
    for ( ; !max_time || end_time > SDL_GetTicks(); ++current_row ) {

        if ( current_row % PRINT_INTERVAL == 0 ) {
            printf( "Raytracing (row %u)...\n", current_row );
        }

        // we're done if we finish the last row
        is_done = current_row == height;
        // break if we finish
        if ( is_done )
            break;

#ifdef FASTER
		if(current_row%2==1)
			continue;
#endif

        for ( size_t x = 0; x < width; ++x ) {

#ifdef FASTER
			if(x%2==1)
				continue;
#endif
            // trace a pixel
            Color3 color = trace_pixel( scene, x, current_row, width, height, intersectionPoint);
            // write the result to the buffer, always use 1.0 as the alpha
            color.to_array( &buffer[4 * ( current_row * width + x )] );

#ifdef FASTER
			color.to_array( &buffer[4 * ( current_row * width + (x+1) )] );
			color.to_array( &buffer[4 * ( (current_row+1) * width + (x) )] );
			color.to_array( &buffer[4 * ( (current_row+1) * width + (x+1) )] );
#endif
        }
    }

    if ( is_done ) {
        printf( "Done raytracing!\n" );
    }

	delete(intersectionPoint);

    return is_done;
}

} /* _462 */

