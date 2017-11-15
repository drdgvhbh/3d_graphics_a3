/*
Ryan Lee - 214240196 - drd
Cheng Shao - 214615934 - shaoc2
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

int IntersectCone(Ray* ray, double* t, Vector* normal) {
	/* your code goes here */

	int missTracker1 =0;
	int missTracker2 =0;

	//flat bottom calculations
	double myT1;
	//find T with y=1
	myT1 = (1.0-ray->origin.v[1])/ray->direction.v[1];

	double x, z;
	//sub in T, find x, and z
	x = ray->origin.v[0]+myT1*ray->direction.v[0];
	z = ray->origin.v[2]+myT1*ray->direction.v[2];

	//is the intersection point outside the circle
	if(pow(x, 2) + pow(z, 2) > 1.0) {
		missTracker2 = 1; //missed
	}


	//cone calculations
	double a =
		+ (pow(ray->direction.v[0], 2))
		- pow(ray->direction.v[1], 2)
		+ pow(ray->direction.v[2], 2);
	double b =
		2 *
		(
			+ (ray->origin.v[0] * ray->direction.v[0])
			- (ray->origin.v[1] * ray->direction.v[1])
			+ ray->origin.v[2] * ray->direction.v[2]
		);
	double c =
		+ (pow(ray->origin.v[0], 2))
		- pow(ray->origin.v[1], 2)
		+ pow(ray->origin.v[2], 2);

	double d;
	double r1, r2;
	double myT;

	Point hit;

	d = pow(b, 2) -4.0 * a *c;
	if (d <= 0.0) {
		missTracker1 = 1;
	}

	if (b < 0.0) {
		r2= (-b+sqrt(d))/(2.0*a);
		r1= c/(a*r2);
	} else {
		r1= (-b-sqrt(d))/(2.0*a);
		r2= c/(a*r1);
	}
	if (r1 < EPSILON) {
		if (r2 < EPSILON) {
			missTracker1 = 1;
		}
		else {
			myT= r1;
		}
	}
	else {
		myT= r2;
	}




	if(missTracker1&&missTracker2) {
		return MISS;
	} else if(missTracker2){
		//do nothing
	} else {
		myT = myT1;
		*t= myT;
		normal->v[0]= 0.0;
		normal->v[1]= 1.0;
		normal->v[2]= 0.0;
		return HIT;

	}

	if (myT >= *t) {
		return MISS;
	}
	double y = ray->origin.v[1] + (myT) * ray->direction.v[1];
	if (y > 1. || y < 0.) {
		return MISS;
	}



	*t= myT;
	Vector normalizedRayDir = ray->direction;
	TIMES(hit, normalizedRayDir, myT);
	PLUS(hit, ray->origin, hit);

	*normal= hit;

	return HIT;
}
