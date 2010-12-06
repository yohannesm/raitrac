/******************************************************************/
/*         Lighting functions                                     */
/*                                                                */
/* Group Members: Yohannes Himawan
 *                David Lee*/
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

bool testShadow = true;

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

bool pointsEqual(point* a, point* b){
return (a->x == b->x) &&  (a->y == b->y) &&  (a->z == b->z);
}

/* shade */
/* color of point p with normal vector n and material m returned in c */
/* in is the direction of the incoming ray and d is the recursive depth */
void shade(point* p, vector* n, material* m, vector* in, color* c, int d, std::vector<light*>& lights) {
if(d==-1) return;

  /* so far, just finds ambient component of color */
  c->r = m->amb * m->r;
  c->g = m->amb * m->g;
  c->b = m->amb * m->b;

for(uint i=0; i<lights.size(); i++){
  // do diffuse
  // this will change to a for loop with multiple lightsa
  //normalize(n);
  ray shadowRay;
  point* LightPt = makePoint(lights[i]->x , lights[i]->y , lights[i]->z );
  shadowRay.dir = makePoint(lights[i]->x - p->x, lights[i]->y - p->y, lights[i]->z - p->z);
  // shadowRay.dir = makePoint(p->x - lights[i]->x, p->y- lights[i]->y, p->z - lights[i]->z);
  
  shadowRay.start = makePoint(p->x + EPSILON *(shadowRay.dir->x)
                               , p->y + EPSILON *(shadowRay.dir->y)
                               , p->z + EPSILON *(shadowRay.dir->z));
                               /*
  shadowRay.start = makePoint(LightPt->x + EPSILON *(shadowRay.dir->x)
                               , LightPt->y + EPSILON *(shadowRay.dir->y)
                               , LightPt->z + EPSILON *(shadowRay.dir->z));*/
  point testPt;
  testPt.w = 1.0;
  bool inLight = (lightHit(&shadowRay, LightPt));

  //if(  pointsEqual(&testPt, LightPt)){
  vector* lightVec = makePoint(lights[i]->x - p->x, lights[i]->y - p->y, lights[i]->z - p->z);
  GLfloat dp = dotProd(lightVec, n) ;
 if( inLight ){

  if (dp < 0)
    dp = 0;
  c->r += m->r * m->dif * dp * (lights[i]->r * lights[i]->i);
  c->g += m->g * m->dif * dp * (lights[i]->g * lights[i]->i);
  c->b += m->b * m->dif * dp * (lights[i]->b * lights[i]->i);
  }
  // do specular
  vector* h = makePoint(in->x + lightVec->x, in->y + lightVec->y, in->z + lightVec->z);
  normalize(h);
  dp = dotProd(h, n);
  if (dp < 0)
    dp = 0;


  GLfloat nl = 2 * dotProd(n, lightVec);
  ray flec;
  flec.dir = makePoint( nl * n->x - lightVec->x, nl * n->y - lightVec->y, nl * n->z - lightVec->z); 
  flec.start = makePoint(p->x + EPSILON *(flec.dir->x) , 
                         p->y + EPSILON *(flec.dir->y),   
                         p->z + EPSILON *(flec.dir->z) );
  color flecColor;
  if(d> 0)
    traceRay(&flec, &flecColor, d-1);
    else{ 
  flecColor.r = 1;//c->r;
  flecColor.g = 1;//c->g;
  flecColor.b = 1;//c->b;
    }
 
  normalize(flec.dir);
  //dp = dotProd(flec.dir, in);
  dp = pow(dp, m->shi);
  c->r += m->spe * dp * flecColor.r *  (lights[i]->r * lights[i]->i);
  c->g += m->spe * dp * flecColor.g * (lights[i]->g * lights[i]->i);
  c->b += m->spe * dp * flecColor.b *  (lights[i]->b * lights[i]->i);

  freePoint(lightVec);
  freePoint(shadowRay.start);
  freePoint(LightPt);
  freePoint(flec.start);
  freePoint(flec.dir);
  
  
}  
  /* clamp color values to 1.0 */
  if (c->r > 1.0) c->r = 1.0;
  if (c->g > 1.0) c->g = 1.0;
  if (c->b > 1.0) c->b = 1.0;



}

