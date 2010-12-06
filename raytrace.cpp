/******************************************************************/
/*         Main raytracer file                                    */
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
#include "lowlevel.h"
#include "raytrace.h"

/* local functions */
void initScene(void);
void initCamera (int, int);
void display(void);
void init(int, int);
void traceRay(ray*,color*);
void drawScene(void);
void firstHit(ray*,point*,vector*,material**);

/* local data */

/* the scene: so far, just one sphere */
sphere* s1;
//ellipsoid * e1;
cylinder* c1;
plane* p1;
std::vector<light*> lights;

/* the viewing parameters: */
point* viewpoint;
GLfloat pnear;  /* distance from viewpoint to image plane */
GLfloat fovx;  /* x-angle of view frustum */
int width = 800;     /* width of window in pixels */
int height = 600;    /* height of window in pixels */

int main (int argc, char** argv) {
  int win;

  glutInit(&argc,argv);
  glutInitWindowSize(width,height);
  glutInitWindowPosition(100,100);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  win = glutCreateWindow("raytrace");
  glutSetWindow(win);
  init(width,height);
  glutDisplayFunc(display);
  glutMainLoop();
  return 0;
}

void init(int w, int h) {

  /* OpenGL setup */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 2.0, 0.0, 2.0, -2.0, 2.0);
  glClearColor(0.0, 0.0, 0.0, 0.0);  

  /* low-level graphics setup */
  initCanvas(w,h);

  /* raytracer setup */
  initCamera(w,h);
  initScene();
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT);
  drawScene();  /* draws the picture in the canvas */
  flushCanvas();  /* draw the canvas to the OpenGL window */
  glFlush();
}

void initScene () {
   //lights.push_back(makeLight(4, 0, -5, 1, 1, 1, .1));
   lights.push_back(makeLight(0.0, 0.5, -1, 1, 1, 1, 1));
   //lights.push_back(makeLight(-3, 3, -2, 1, 1, 1, .1));
   //lights.push_back(makeLight(5, 2, -0.5, 0.5, 0.5, 0.5, .1));
   s1 = makeSphere(0.0,0.0,-3.0,0.1);
   c1 = makeCylinder(-0.5, 0.0, -2.0, 
                     0.1);
   p1 = makePlane( 0, 1, 0,   0.0, -.3, -1.0);
  //e1 = makeEllipsoid(-0.0,0.0,-2.0,0.25, 0.0, 0.0, -2.0, 0.36);
  s1->m = makeMaterial(0.0,0.1,1 ,0.3,0.9,1,125);
  c1->m = makeMaterial(0.0,0.1,1 ,0.5,0.9,0,125);
  //c1->m = makeMaterial(0.0,0.9,0.1 , 0.5,0.5, 0.5,1);
  p1->m = makeMaterial(0.7,0.7,0.7 , 0.4,0.2,0.3,1);
  //e1->m = makeMaterial(0.0,0.1,1,0.3,0.9,1,125);

  
}

void initCamera (int w, int h) {
  viewpoint = makePoint(0.0,0.0,0.0);
  pnear = 1.0;
  fovx = PI/6;
}

void drawScene () {
  int i,j;
  GLfloat imageWidth;
  /* declare data structures on stack to avoid dynamic allocation */
  point worldPix;  /* current pixel in world coordinates */
  point direction; 
  ray r;
  color c;

  /* initialize */
  worldPix.w = 1.0;
  worldPix.z = -pnear;

  r.start = &worldPix;
  r.dir= &direction;

  imageWidth = 2*pnear*tan(fovx/2);

  /* trace a ray for every pixel */
  for (i=0; i<width; i++) {
    /* Refresh the display */
    /* Comment this line out after debugging */
    flushCanvas();

    for (j=0; j<height; j++) {

      /* find position of pixel in world coordinates */
      /* y position = (pixel height/middle) scaled to world coords */ 
      worldPix.y = (j-(height/2))*imageWidth/width;
      /* x position = (pixel width/middle) scaled to world coords */ 
      worldPix.x = (i-(width/2))*imageWidth/width;

      /* find direction */
      /* note: direction vector is NOT NORMALIZED */
      calculateDirection(viewpoint,&worldPix,&direction);

      /* trace the ray! */
      traceRay(&r,&c,0);
      /* write the pixel! */
      drawPixel(i,j,c.r,c.g,c.b);
    }
  }
}

/* returns the color seen by ray r in parameter c */
/* d is the recursive depth */
//color = color intensity of the ray
void traceRay(ray* r, color* c, int d) {
  point p;  /* first intersection point */
  vector n;
  material* m;

  p.w = 0.0;  /* inialize to "no intersection" */
  firstHit(r,&p,&n,&m);

  if (p.w != 0.0) {
    shade(&p,&n,m,r->dir,c,d,lights);  /* do the lighting calculations */
  } else {             /* nothing was hit */
    c->r = 0.0;
    c->g = 0.0;
    c->b = 0.0;
  }
}

int findMin(double  ttemp[], int hits[], int size){
   int index = -1;
   double min = ttemp[0];
   bool found = false;
   for(int i =0; i< size ; ++i){
     if(ttemp[i] <= min && hits[i] == TRUE  || !found && hits[i] == TRUE){
      min = ttemp[i];
      index = i;
     } 
   found |= (hits[i] == 1);
   }
   return index;
}

/* firstHit */
/* If something is hit, returns the finite intersection point p, 
   the normal vector n to the surface at that point, and the surface
   material m. If no hit, returns an infinite point (p->w = 0.0) */
void firstHit(ray* r, point* p, vector* n, material* *m) {
  double ttemp[3] = {0, 0, 0};
  int hits[3] = {0, 0, 0};
  int boundSphere = 0;
  int boundCyl = 1;
   int boundPlane = 2;
  hits[0] = raySphereIntersect(r,s1,&ttemp[0]);
  hits[1] =  rayCylinderIntersect(r, c1, &ttemp[1]);
  hits[2] =  rayPlaneIntersect(r, p1, &ttemp[2]);
  int val = findMin(ttemp, hits, 3);
  double t = 0;
  if(val!= -1){
    t = ttemp[val];
      findPointOnRay(r,t,p);
    if(val <= boundSphere){
      *m = s1->m;
      findSphereNormal(s1,p,n);
   }
   else if(val <= boundCyl){
    *m = c1->m;
    findCylinderNormal(c1,p,n);
  }
   else if(val <= boundPlane){
    *m = p1->m;
    findPlaneNormal(p1,p,n);
   } 
   else {
    /* indicates no hit */
    p->w = 0.0;
  }
    }
  else {
    /* indicates no hit */
    p->w = 0.0;
  }

}

bool lightHit(ray* r, point* p ){
  bool res = true;
  vector* test = makePoint(p->x - r->start->x, p->y - r->start->y, p->z - r->start->z); 
  normalize(test);
  //vector* dir = makePoint(r->dir->x, r->dir->y, r->dir->z); 
  //normalize(dir);
  GLfloat tx = test->x / r->dir->x;
  GLfloat ty = test->y / r->dir->y;
  GLfloat tz = test->z / r->dir->z;
  if( tx == ty  && ty == tz){
   double ttemp[3] = {0, 0, 0};
  int hits[3] = {0, 0, 0};
  int boundSphere = 0;
  int boundCyl = 1;
   int boundPlane = 2;
  hits[0] = raySphereIntersect(r,s1,&ttemp[0]);
  hits[1] =  rayCylinderIntersect(r, c1, &ttemp[1]);
  hits[2] =  rayPlaneIntersect(r, p1, &ttemp[2]);
  
  for(int i=0; i< 3; ++i){
    if(ttemp[i] < tx && hits[i] == TRUE){
    res = false;
    break;
    }
  }
  freePoint(test);
}
 return res;
}

