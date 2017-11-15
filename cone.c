/*
Ryan Lee - 214240196 - drd
Cheng Shao - 214615934 - shaoc2
 */


#include <math.h>
#include "artInternal.h"

extern double Normalize();

#define EPSILON	(1e-10)

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

	double d, myT;
	double r1, r2;
	Point hit;

	d = pow(b, 2) -4.0 * a *c;
	if (d <= 0.0) {
		return MISS;
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
			return MISS;
		}
		else {
			myT= r2;
		}
	}
	else { 
		myT= r1;
	}

	if (myT >= *t) {
		return MISS;
	}
	if (ray->origin.v[1] + (myT) * ray->direction.v[1] > 1
		|| ray->origin.v[1] + (myT) * ray->direction.v[1] < 0) {
			return MISS;
		}
	TIMES(hit,  ray->direction, myT);
	PLUS(hit, ray->origin, hit);
	*t= myT;
	*normal= hit;
	
	return HIT;
}
