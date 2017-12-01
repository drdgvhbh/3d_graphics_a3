/*
 * implement ray/cone intersection routine
 *
 *	John Amanatides, Oct 2017
 */

#include <math.h>
#include "artInternal.h"

extern double Normalize();

#define EPSILON	(1e-10)

typedef struct {
        Vector normal;
        double d;
} Plane;

//the y=1 surface of the cone
static Plane plane = {0.0, 1.0, 0.0, -1.0};
/*
static double IntersectPlane(Ray *ray, Plane plane)
{
	double a, b;

	a= DOT(ray->direction, plane.normal);
	b= DOT(ray->origin, plane.normal)+plane.d;

	return -b/a;
}
*/
/*
 * compute intersection between ray and a cone (0<= y <= 1, x^2 + z^2 <= y^2)
 * Returns MISS if no intersection; otherwise, it returns HIT and
 * sets t to the the distance  to the intersection point and sets the
 * normal vector to the outward facing normal (unit length) at the
 * intersection point.  Note: no intersection is returned if myT >= t
 * (ie my intersection point is further than something else already hit).
 */

int IntersectCone(ray, t, normal)
	Ray *ray;
	double *t;
	Vector *normal;
	
{
	Vector thirdElementNegative;
	double thirdElement, secondElement, firstElement;
	double a, b, c;
	double d, myT;
	double r1, r2;
	double myT;

	Point hit;
	Point conePoint;
	double yValueCone, xValueCone, zValueCone;
	double xz;
	
	
	
	//--------------------------------------------------------------------------- 
	// create new vectors
	// give vectors a negative y value to correspond to equation
   Vector newRD, newRO;
   newRD = ray->direction; 
   newRD.v[1] = ray->direction.v[1] * -1.0;
   newRO = ray->origin;
   newRO.v[1] = ray->origin.v[1] * -1.0;
     
     a= DOT(ray->direction, newRD);
     b= 2.0*DOT(ray->origin, newRD );
     c= DOT(ray->origin, newRO); 

	//----------------------------------------------------------------------------
	d= b*b-4.0*a*c;
	if(d <= 0.0)
		return MISS;
	//find roots
	if(b < 0.0) {
		r2= (-b+sqrt(d))/(2.0*a);
		r1= c/(a*r2);
	} else {
		r1= (-b-sqrt(d))/(2.0*a);
		r2= c/(a*r1);
	}

// find smallest radius
	if (r1 < r2)
	{
		myT = r1;
	}
	else if (r2 < r1)
	{
		myT = r2;
	}

	if(myT < EPSILON)
	{
		return MISS;
	}
		// find the y value of myT
	TIMES(conePoint,ray->direction, myT);
	PLUS(conePoint,conePoint, ray->origin);
	xValueCone = conePoint.v[0];
	yValueCone = conePoint.v[1];
	zValueCone = conePoint.v[2];
	xz = yValueCone + zValueCone;
	
	//if object in front of eye ahead of cone, or bottom section of cone
	if(myT >= *t || yValueCone < 0.0){
		return MISS;
	}else if(yValueCone>1.0){
		//if looking inside the cone from the top
		if((ray->origin.v[1]>1) && ray->direction.v[1]<0){
			//draw plane on top of cone
			myT=-((ray->origin.v[1]-1)/ray->direction.v[1]);
			TIMES(hit,  ray->direction, myT);
			PLUS(hit, ray->origin, hit);
			//keep plane restricted to a circle
			if(pow(hit.v[0],2)+pow(hit.v[2],2)>1){
				return MISS;
			}else{
				return HIT;
			}
		}




	}else{
	//regular cone on the ouside intersection
	
	TIMES(hit,  ray->direction, myT);
	PLUS(hit, ray->origin, hit);
	*t= myT;
	hit.v[1] = hit.v[1] * -1;
	*normal= hit;
	return HIT;
	}
	
	
}
