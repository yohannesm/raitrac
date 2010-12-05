#ifndef _RAYTRACE_H_
#define _RAYTRACE_H_

/******************************************************************/
/*         Raytracer declarations                                 */
/******************************************************************/


/* constants */
#define TRUE 1
#define FALSE 0

#define PI 3.14159265358979323846264338327
#include <vector>



/* data structures */

typedef struct point {
  GLfloat x;
  GLfloat y;
  GLfloat z;
  GLfloat w;
} point;

/* a vector is just a point */
typedef point vector;

/* a ray is a start point and a direction */
typedef struct ray {
  point* start;
  vector* dir;
} ray;

//a line is just a ray
typedef ray line;

typedef struct material {
  /* color */
  GLfloat r;
  GLfloat g;
  GLfloat b; 
  /* ambient reflectivity */
  GLfloat amb;
  GLfloat dif;
  GLfloat spe;
  GLfloat shi;
} material;

typedef struct color {
  GLfloat r;
  GLfloat g;
  GLfloat b; 
  /* these should be between 0 and 1 */
} color;

typedef struct sphere {
  point* c;  /* center */
  GLfloat r;  /* radius */
  material* m;
} sphere;

typedef struct ellipsoid{
   point* f1; //focal pt 1
   point* f2; //focal pt 2
   GLfloat a;
   GLfloat b;
   material * m;
} ellipsoid;

typedef struct cylinder{
   line* c; // center of cylinder
   GLfloat r; //radius of the cylinder
   material* m;
}cylinder;

typedef struct light {
  GLfloat x;
  GLfloat y;
  GLfloat z;
  
  GLfloat r;
  GLfloat g;
  GLfloat b; 
  
  GLfloat i;
} light;



/* functions in raytrace.cpp */
void traceRay(ray*, color*, int);

/* functions in geometry.cpp */
GLfloat dotProd (vector* v0, vector* v1);
void normalize(vector* v);
sphere* makeSphere(GLfloat, GLfloat, GLfloat, GLfloat);
ellipsoid* makeEllipsoid(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
cylinder* makeCylinder(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
point* makePoint(GLfloat, GLfloat, GLfloat);
point* copyPoint(point *);
void freePoint(point *);
void calculateDirection(point*,point*,point*);
void findPointOnRay(ray*,double,point*);
int raySphereIntersect(ray*,sphere*,double*);
void findSphereNormal(sphere*,point*,vector*);
int rayEllipsoidIntersect(ray*,ellipsoid*,double*);
void findEllipsoidNormal(ellipsoid*,point*,vector*);

/* functions in light.cpp */
light* makeLight(GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat g, GLfloat b, GLfloat i);
material* makeMaterial(GLfloat r, GLfloat g, GLfloat b, GLfloat amb, GLfloat dif, GLfloat spe, GLfloat shi);
void shade(point*,vector*,material*,vector*,color*,int, std::vector<light*>&);

/* global variables */
extern int width;
extern int height;

#endif	/* _RAYTRACE_H_ */
