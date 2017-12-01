/*
* the scene data structure is created/stored/traversed here
*
*	John Amanatides, Oct 2017
*/


#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include "artInternal.h"
#include <stdio.h>

extern Material	GetCurrentMaterial(void);
extern int	IntersectSphere(Ray *, double *, Vector *);
extern int	IntersectPlane(Ray *, double *, Vector *);
extern int	IntersectCube(Ray *, double *, Vector *);
extern int	IntersectCone(Ray *, double *, Vector *);
extern Point	InvTransPoint(Point, Affine *);
extern Vector	InvTransVector(Vector, Affine *), TransNormal(Vector, Affine *);
extern Matrix	MultMatrix(Matrix *, Matrix *);
extern void	InitCamera(void), InitLighting(void), FinishLighting(void);

#define SPHERE		1
#define PLANE		2
#define CUBE		3
#define CONE	4
#define PI 3.14159265

typedef struct StackNode {
	Affine CTM;
	struct StackNode *next;
} StackNode;

typedef struct ListNode {
	int nodeType;
	Affine affine;
	Material material;
	struct ListNode *next;
} ListNode;

static Matrix identity= {       1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
0.0, 0.0, 0.0, 1.0};
static Affine CTM;
static StackNode *CTMstack;
static ListNode *scene;


char *
art_Start(void)
{
	CTM.TM= identity;
	CTM.inverseTM= identity;
	CTMstack= NULL;
	InitCamera();
	InitLighting();
	scene= NULL;
	
	return NULL;
}


static void
FreeModel(scene)
ListNode *scene;
{
	ListNode *node;
	while(scene) {
		node= scene;
		scene= scene->next;
		free((void *) node);
		/* note material node is never removed */
	}
}


char *
art_End(void)
{
	while(CTMstack != NULL)
		(void) art_PopTM();
	FreeModel(scene);
	FinishLighting();
	return NULL;
}


char *
art_InitTM(void)
{
	CTM.TM= identity;
	CTM.inverseTM= identity;
	return NULL;
}


char *
art_PushTM(void)
{
	StackNode *sp;
	
	sp= (StackNode *) malloc(sizeof(StackNode));
	sp->CTM= CTM;
	sp->next= CTMstack;
	CTMstack= sp;
	return NULL;
}


char *
art_PopTM(void)
{
	StackNode *sp;
	
	if(CTMstack != NULL) {
		CTM= CTMstack->CTM;
		sp= CTMstack;
		CTMstack= CTMstack->next;
		free((void *) sp);
		return NULL;
	}
	else	return "stack underflow";
}


/* premultiply CTM */
static void
ApplyAffine(Affine trans)
{
	CTM.TM= MultMatrix(&trans.TM, &CTM.TM);
	CTM.inverseTM= MultMatrix(&CTM.inverseTM, &trans.inverseTM);
}

//scale xyz value 
char *
art_Scale(double sx, double sy, double sz)
{
    Affine trans; 
    
	Matrix a = {sx, 0.0,0.0,0.0,
		0.0,sy,0.0,0.0,
		0.0,0.0,sz,0.0,
	0.0,0.0,0.0,1.0};
	
	Matrix aInverse = {1/sx, 0.0,0.0,0.0,
		0.0,1/sy,0.0,0.0,
		0.0,0.0,1/sz,0.0,
	0.0,0.0,0.0,1.0};
	
	trans.TM = a;
	trans.inverseTM = aInverse;		
	ApplyAffine(trans);
	
	
	return NULL;
	
}

//rotate in specefic axis
char *
art_Rotate(char axis, double degrees)
{
	Affine rotateX;
	Affine rotateY;
	Affine rotateZ;
	/* your code goes here */
	double val = PI / 180;
	double sinD = sin(degrees*val);
	double cosD = cos(degrees*val);
	
	if(axis == 'x') 
	{
		//rotate is x axis
		Matrix xAxis = {1.0, 0.0, 0.0, 0.0,
						0.0, cosD, -sinD, 0.0,
						0.0, sinD, cosD, 0.0,
						0.0, 0.0, 0.0, 1.0};
		
		Matrix xInverse = {1.0, 0.0, 0.0, 0.0,
							0.0, cosD, sinD, 0.0,
							0.0, -sinD, cosD, 0.0,
							0.0, 0.0, 0.0, 1.0};
		
		rotateX.TM= xAxis;
		rotateX.inverseTM= xInverse;
		ApplyAffine(rotateX);
		
	}
	else if (axis == 'y') 
	{
		//rotate in y axis 
		Matrix yAxis = {cosD, 0.0, sinD, 0.0,
			0.0, 1.0, 0.0, 0.0,
			-sinD, 0.0, cosD, 0.0,
		0.0, 0.0, 0.0, 1.0};
		
		Matrix yInverse = {cosD, 0.0, -sinD, 0.0,
			0.0, 1.0, 0.0, 0.0,
			sinD, 0.0, cosD, 0.0,
		0.0, 0.0, 0.0, 1.0};
		
		rotateY.TM=yAxis;
		rotateY.inverseTM=yInverse;
		ApplyAffine(rotateY);
		
	}
	else if (axis == 'z')
	{
		//rotate in z axis 
		Matrix zAxis = {cosD, -sinD, 0.0, 0.0,
			sinD, cosD, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0};
		
		Matrix zInverse = {cosD, sinD, 0.0, 0.0,
			-sinD, cosD, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0}; 	
		
		rotateZ.TM=zAxis;
		rotateZ.inverseTM=zInverse;
		ApplyAffine(rotateZ);
		
	}
	
	
	
	return NULL;
}

//translate according to xyz value
char *
art_Translate(double tx, double ty, double tz)
{
	
	Affine trans1; 
    
	Matrix b = {1.0, 0.0,0.0,tx,
		0.0,1.0,0.0,ty,
		0.0,0.0,1.0,tz,
		0.0,0.0,0.0,1.0};
	
	Matrix bInverse = {1.0, 0.0,0.0,-tx,
		0.0,1.0,0.0,-ty,
		0.0,0.0,1.0,-tz,
		0.0,0.0,0.0,1.0};
	
	trans1.TM = b;
	trans1.inverseTM = bInverse;		
	ApplyAffine(trans1);
	return NULL;
}


// shear in 2 axis
char *
art_Shear(char axis1, char axis2, double shear)
{
	/* your code goes here */
	
	Affine shearTM; 
	if (axis1 == 'x')
	{
		if(axis2 == 'y')
		{
		Matrix matrix_XOfY = {1.0, -shear,0.0,0.0,
				0.0,1.0,0.0,0.0,
				0.0,0.0,1.0,0.0,
				0.0,0.0,0.0,1.0};
				
		Matrix matrix_XOfYInverse = {1.0, shear,0.0,0.0,
						   0.0,1.0,0.0,0.0,
				           0.0,0.0,1.0,0.0,
				           0.0,0.0,0.0,1.0};
			
			shearTM.TM = matrix_XOfY;
			shearTM.inverseTM = matrix_XOfY;
		}
		
		if(axis2 == 'z')
		{
			Matrix matrix_XOfZ = {1.0,0.0,-shear,0.0,
				0.0,1.0,0.0,0.0,
				0.0,0.0,1.0,0.0,
				0.0,0.0,0.0,1.0};
				
		Matrix matrix_XOfZInverse = {1.0, 0.0,shear,0.0,
						   0.0,1.0,0.0,0.0,
				           0.0,0.0,1.0,0.0,
				           0.0,0.0,0.0,1.0};
				           
	   shearTM.TM = matrix_XOfZ;
	   shearTM.inverseTM = matrix_XOfZ;
		}
		
	}
	
	else if (axis1 == 'y')
	{
		if(axis2 == 'x')
		{
			Matrix matrix_YOfX = {1.0, 0.0,0.0,0.0,
						   -shear,1.0,0.0,0.0,
				           0.0,0.0,1.0,0.0,
				           0.0,0.0,0.0,1.0};
				           
		    Matrix matrix_YOfXInverse = {1.0, 0.0,0.0,0.0,
						   shear,1.0,0.0,0.0,
				           0.0,0.0,1.0,0.0,
				           0.0,0.0,0.0,1.0};
				           
		    shearTM.TM = matrix_YOfX;
			shearTM.inverseTM = matrix_YOfX;
		}
		
		if(axis2 == 'z')
		{
			Matrix matrix_YOfZ = {1.0, 0.0,0.0,0.0,
						   0.0,1.0,-shear,0.0,
				           0.0,0.0,1.0,0.0,
				           0.0,0.0,0.0,1.0};
				           
		    Matrix matrix_YOfZInverse = {1.0, 0.0,0.0,0.0,
						   0.0,1.0,shear,0.0,
				           0.0,0.0,1.0,0.0,
				           0.0,0.0,0.0,1.0};
				           
			shearTM.TM = matrix_YOfZ;
			shearTM.inverseTM = matrix_YOfZ;
		}
		
	}
	
	else if (axis1 == 'z')
	{
		if(axis2 == 'x')
		{
			Matrix matrix_ZOfX = {1.0, 0.0,0.0,0.0,
						   0.0,1.0,0.0,0.0,
				           -shear,0.0,1.0,0.0,
				           0.0,0.0,0.0,1.0};
				           
		    Matrix matrix_ZOfXInverse = {1.0, 0.0,0.0,0.0,
						   0.0,1.0,0.0,0.0,
				           shear,0.0,1.0,0.0,
				           0.0,0.0,0.0,1.0};
		  shearTM.TM = matrix_ZOfX;
		  shearTM.inverseTM = matrix_ZOfX;
		}
		
		if(axis2 == 'y')
		{
			Matrix matrix_ZOfY = {1.0, 0.0,0.0,0.0,
						   0.0,1.0,0.0,0.0,
				           0.0,-shear,1.0,0.0,
				           0.0,0.0,0.0,1.0};
				           
		    Matrix matrix_ZOfYInverse = {1.0, 0.0,0.0,0.0,
						   0.0,1.0,0.0,0.0,
				           0.0,shear,1.0,0.0,
				           0.0,0.0,0.0,1.0};
				           
	      shearTM.TM = matrix_ZOfY;
		  shearTM.inverseTM = matrix_ZOfY;
		}
		
	}
	ApplyAffine(shearTM);
	return NULL;
}


static void AddObject(int nodeType)
{
	ListNode *object;
	
	object= (ListNode *) malloc (sizeof(ListNode));
	object->nodeType= nodeType;
	object->affine= CTM;
	object->material= GetCurrentMaterial();
	object->next= scene;
	scene= object;
}


char *
art_Sphere()
{
	AddObject(SPHERE);
	return NULL;
}


char *
art_Plane()
{
	AddObject(PLANE);
	return NULL;
}


char *
art_Cube()
{
	AddObject(CUBE);
	return NULL;
}

char *
art_Cone()
{
	AddObject(CONE);
	return NULL;
}


/*
* This function, when passed a ray and list of objects
* returns a pointer to the closest intersected object in the list
* (whose t-value is less than t) and updates t and normal to
* the value of the closest object's normal and t-value.
* It returns NULL if it find no object closer than t from
* the ray origin.  If anyHit is true then it returns the
* first object that is closer than t.
*/
static ListNode *
ReallyIntersectScene(Ray *ray, ListNode *obj, int anyHit, double *t, Vector *normal)
{
	ListNode *closestObj, *resultObj;
	Ray transRay;
	int i, result;
	
	closestObj= NULL;
	
	while(obj != NULL) {
		/* transform ray */
		transRay.origin= InvTransPoint(ray->origin, &obj->affine);
		transRay.direction= InvTransVector(ray->direction, &obj->affine);
		
		/* intersect object */
		switch(obj->nodeType) {
			
		case SPHERE:
			result= IntersectSphere(&transRay, t, normal);
			break;
		case PLANE:
			result= IntersectPlane(&transRay, t, normal);
			break;
		case CUBE:
			result= IntersectCube(&transRay, t, normal);
			break;
		case CONE:
			result= IntersectCone(&transRay, t, normal);
			break;
		}
		
		/* keep closest intersection */
		if (result == HIT) {
			closestObj= obj;
			if(anyHit)
				return obj;
		}
		
		obj= obj->next;
	}
	
	return closestObj;
}


int
IntersectScene(Ray *ray, double *t, Vector *normal, Material *material)
{
	ListNode *closestObj;
	double closestT;
	Vector closestNormal;
	
	closestT= UNIVERSE;
	closestObj= ReallyIntersectScene(ray, scene, 0, &closestT, &closestNormal);
	if(closestObj != NULL) {
		*t= closestT;
		*normal= TransNormal(closestNormal, &closestObj->affine);
		*material= closestObj->material;
		return HIT;
	}
	else	return MISS;
}


int
ShadowProbe(Ray *ray, double distanceToLight)
{
	ListNode *closestObj;
	double closestT;
	Vector closestNormal;
	
	closestT= distanceToLight;
	closestObj= ReallyIntersectScene(ray, scene, 1, &closestT, &closestNormal);
	if(closestObj != NULL)
		return HIT;
	else	return MISS;
}
