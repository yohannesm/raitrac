/******************************************************************/
/*         Geometry functions                                     */
/*                                                                */
/* Group Members: <FILL IN>                                       */
/******************************************************************/

#ifdef _WIN32
#include <windows.h>
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "common.h"
#include "raytrace.h"

point* makePoint(GLfloat x, GLfloat y, GLfloat z) {
  point* p;
  /* allocate memory */
  p = (point*) malloc(sizeof(point));
  /* put stuff in it */
  p->x = x; p->y = y; p->z = z; 
  p->w = 1.0;
  return (p);
}

/* makes copy of point (or vector) */
point* copyPoint(point *p0) {
  point* p;
  /* allocate memory */
  p = (point*) malloc(sizeof(point));

  p->x = p0->x;
  p->y = p0->y;
  p->z = p0->z;
  p->w = p0->w;         /* copies over vector or point status */
  return (p);
}

/* unallocates a point */
void freePoint(point *p) {
  if (p != NULL) {
    free(p);
  }
}

/* vector from point p to point q is returned in v */
void calculateDirection(point* p, point* q, point* v) {
  v->x = q->x - p->x;
  v->y = q->y - p->y;
  v->z = q->z - p->z;
  /* a direction is a point at infinity */
  v->w = 0.0;

  /* NOTE: v is not unit length currently, but probably should be */
}

/* given a vector, sets its contents to unit length */
void normalize(vector* v) {
	GLfloat mag = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
	v->x /= mag;
	v->y /= mag;
	v->z /= mag;
}

GLfloat dotProd (vector* v0, vector* v1)
{
  GLfloat result = v0->x * v1->x + v0->y * v1->y + v0->z * v1->z;
  return result;
}

//must call freePoint after this call 
vector* crossProd(vector* v0, vector* v1){
  vector* result = makePoint( v0->y * v1->z - v0->z * v1->y,
                              v0->z * v1->x - v0->x * v1->z,
                              v0->x * v1->y - v0->y * v1->x);
  return result;
}

/* point on ray r parameterized by t is returned in p */
void findPointOnRay(ray* r,double t,point* p) {
  p->x = r->start->x + t * r->dir->x;
  p->y = r->start->y + t * r->dir->y;
  p->z = r->start->z + t * r->dir->z;
  p->w = 1.0;
}


/* SPHERES */

sphere* makeSphere(GLfloat x, GLfloat y, GLfloat z, GLfloat r) {
  sphere* s;
  /* allocate memory */
  s = (sphere*) malloc(sizeof(sphere));

  /* put stuff in it */
  s->c = makePoint(x,y,z);   /* center */
  s->r = r;   /* radius */
  s->m = NULL;   /* material */
  return(s);
}

ellipsoid* makeEllipsoid (GLfloat x1, GLfloat y1, GLfloat z1, GLfloat a,
                          GLfloat x2, GLfloat y2, GLfloat z2, GLfloat b) {
  ellipsoid* e;
  /* allocate memory */
  e = (ellipsoid*) malloc(sizeof(ellipsoid));

  /* put stuff in it */
  e->f1 = makePoint(x1, y1, z1);
  e->f2 = makePoint(x2, y2, z2);
  e->a = a;   /* radius */
  e->b = b;   /* radius */
  e->m = NULL;   /* material */
  return(e);
}

cylinder* makeCylinder(GLfloat x1,GLfloat y1,GLfloat z1,
                       GLfloat x2,GLfloat y2,GLfloat z2, GLfloat r){
  cylinder* cyl;
  cyl = (cylinder*) malloc(sizeof(cylinder));

  cyl->c->start = makePoint(x1, y1, z1);
  cyl->c->dir   = makePoint(x2, y2, z2);
  cyl->r = r; //radius
  cyl->m = NULL;
  return cyl;

}



/* returns TRUE if ray r hits sphere s, with parameter value in t */
int raySphereIntersect(ray* r,sphere* s,double* t) {
  point p;   /* start of transformed ray */
  double a,b,c;  /* coefficients of quadratic equation */
  double D;    /* discriminant */
  point* v;
  
  /* transform ray so that sphere center is at origin */
  /* don't use matrix, just translate! */
  p.x = r->start->x - s->c->x;
  p.y = r->start->y - s->c->y;
  p.z = r->start->z - s->c->z;
  v = r->dir; /* point to direction vector */


  a = v->x * v->x  +  v->y * v->y  +  v->z * v->z;
  b = 2*( v->x * p.x  +  v->y * p.y  +  v->z * p.z);
  c = p.x * p.x + p.y * p.y + p.z * p.z - s->r * s->r;

  D = b * b - 4 * a * c;
  
  if (D < 0) {  /* no intersection */
    return (FALSE);
  }
  else {
    D = sqrt(D);
    /* First check the root with the lower value of t: */
    /* this one, since D is positive */
    *t = (-b - D) / (2*a);
    /* ignore roots which are less than zero (behind viewpoint) */
    if (*t < 0) {
      *t = (-b + D) / (2*a);
    }
    if (*t < 0) { return(FALSE); }
    else return(TRUE);
  }
}

/* normal vector of s at p is returned in n */
/* note: dividing by radius normalizes */
void findSphereNormal(sphere* s, point* p, vector* n) {
  n->x = (p->x - s->c->x) / s->r;  
  n->y = (p->y - s->c->y) / s->r;
  n->z = (p->z - s->c->z) / s->r;
  n->w = 0.0;
}
/*
vector* normalize(vector* v){
    GLfloat mag = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
    vector* result = makePoint(v->x/mag, v->y/mag, v->z/mag);
    return result;
}*/



int rayEllipsoidIntersect(ray* r,ellipsoid* e,double* t) {
  vector p1, p2;   /* start of transformed ray */
  double a,b,c;  /* coefficients of quadratic equation */
  double D;    /* discriminant */
  point* d;

  double a2 = e->a * e->a;
  double b2 = e->b * e->b;
  double a2b2 = a2 * b2;
  
  /* transform ray so that ellipsoid center is at origin */
  /* don't use matrix, just translate! */
  p1.x = r->start->x - e->f1->x;
  p1.y = r->start->y - e->f1->y;
  p1.z = r->start->z - e->f1->z;
  p2.x = r->start->x - e->f2->x;
  p2.y = r->start->y - e->f2->y;
  p2.z = r->start->z - e->f2->z;
  d = makePoint(r->dir->x, r->dir->y, r->dir->z);
  normalize(d);

  a = ( e->a * e->a  +  e->b * e->b) / a2b2;
  b = b2 * ( 2*dotProd( &p1, d) ) + a2 * (2*dotProd(&p2, d))   ; 
  b /= a2b2;
  c = b2 * (dotProd(&p1, &p1) ) + a2 * (dotProd(&p2, &p2))   ; 
  c = c / a2b2 - 1;

  D = b * b - 4 * a * c;
  
  freePoint(d);
  if (D < 0) {  /* no intersection */
    return (FALSE);
  }
  else {
    D = sqrt(D);
    /* First check the root with the lower value of t: */
    /* this one, since D is positive */
    *t = (-b - D) / (2*a);
    /* ignore roots which are less than zero (behind viewpoint) */
    if (*t < 0) {
      *t = (-b + D) / (2*a);
    }
    if (*t < 0) { return(FALSE); }
    else return(TRUE);
  }
}

/* normal vector of s at p is returned in n */
void findEllipsoidNormal(ellipsoid* e, point* p, vector* n) {
  n->x = (2*p->x) / (e->a * e->a);  
  n->y = (2*p->y) / (e->b * e->b);  
  n->z = (2*p->z);
  n->w = 0.0;
}


#if 0
int rayCylinderIntersect(ray* r,cylinder* cyl,double* t) {
  vector p1, p2;   /* start of transformed ray */
  double a,b,c;  /* coefficients of quadratic equation */
  double D;    /* discriminant */
  point* d;

  double a2 = e->a * e->a;
  double b2 = e->b * e->b;
  double a2b2 = a2 * b2;
  
  /* transform ray so that cylinder center is at origin */
  /* don't use matrix, just translate! */
  p1.x = r->start->x - e->f1->x;
  p1.y = r->start->y - e->f1->y;
  p1.z = r->start->z - e->f1->z;
  p2.x = r->start->x - e->f2->x;
  p2.y = r->start->y - e->f2->y;
  p2.z = r->start->z - e->f2->z;
  d = makePoint(r->dir->x, r->dir->y, r->dir->z);
  normalize(d);

  a = ( e->a * e->a  +  e->b * e->b) / a2b2;
  b = b2 * ( 2*dotProd( &p1, d) ) + a2 * (2*dotProd(&p2, d))   ; 
  b /= a2b2;
  c = b2 * (dotProd(&p1, &p1) ) + a2 * (dotProd(&p2, &p2))   ; 
  c = c / a2b2 - 1;

  D = b * b - 4 * a * c;
  
  freePoint(d);
  if (D < 0) {  /* no intersection */
    return (FALSE);
  }
  else {
    D = sqrt(D);
    /* First check the root with the lower value of t: */
    /* this one, since D is positive */
    *t = (-b - D) / (2*a);
    /* ignore roots which are less than zero (behind viewpoint) */
    if (*t < 0) {
      *t = (-b + D) / (2*a);
    }
    if (*t < 0) { return(FALSE); }
    else return(TRUE);
  }
}

/* normal vector of s at p is returned in n */
//calculate the closest point here. 
void findCylinderNormal(cylinder* c, point* p, vector* n) {
  n->x = (p->x - c->c->x) / c->r;  
  n->y = (p->y - s->c->y) / c->r;
  n->z = (p->z - s->c->z) / c->r;
  n->w = 0.0;
}
#endif

