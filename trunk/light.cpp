/******************************************************************/
/*         Lighting functions                                     */
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

light* makeLight(GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat g, GLfloat b, GLfloat i)
{
  light* l;
  l = (light*) malloc(sizeof(light));

  // define loc
  l->x = x;
  l->y = y;
  l->z = z;
  
  // define color
  l->r = r;
  l->g = g;
  l->b = b;
  
  l->i = i;
  
  return l;
}

material* makeMaterial(GLfloat r, GLfloat g, GLfloat b, GLfloat amb, GLfloat dif, GLfloat spe, GLfloat shi) {
  material* m;
  
  /* allocate memory */
  m = (material*) malloc(sizeof(material));
  /* put stuff in it */
  m->r = r;
  m->g = g;
  m->b = b;
  m->amb = amb;
  m->dif = dif;
  m->spe = spe;
  m->shi = shi;
  return(m);
}

/* LIGHTING CALCULATIONS */

/* shade */
/* color of point p with normal vector n and material m returned in c */
/* in is the direction of the incoming ray and d is the recusive depth */
void shade(point* p, vector* n, material* m, vector* in, color* c, int d, light* l1) {

  /* so far, just finds ambient component of color */
  c->r = m->amb * m->r;
  c->g = m->amb * m->g;
  c->b = m->amb * m->b;
  
  // do diffuse
  // this will change to a for loop with multiple lights
  vector* lightVec = makePoint(l1->x - p->x, l1->y - p->y, l1->z - p->z);
  GLfloat dp = dotProd(lightVec, n);
  if (dp < 0)
    dp = 0;
  c->r += m->dif * dp * (l1->r * l1->i);
  c->g += m->dif * dp * (l1->g * l1->i);
  c->b += m->dif * dp * (l1->b * l1->i);
  
  // do specular
  vector* h = makePoint(in->x + lightVec->x, in->y + lightVec->y, in->z + lightVec->z);
  normalize(h);
  dp = dotProd(h, n);
  if (dp < 0)
    dp = 0;
  dp = pow(dp, m->shi);
  c->r += m->spe * dp * (l1->r * l1->i);
  c->g += m->spe * dp * (l1->g * l1->i);
  c->b += m->spe * dp * (l1->b * l1->i);
  
  /* clamp color values to 1.0 */
  if (c->r > 1.0) c->r = 1.0;
  if (c->g > 1.0) c->g = 1.0;
  if (c->b > 1.0) c->b = 1.0;
  
  freePoint(lightVec);

}

