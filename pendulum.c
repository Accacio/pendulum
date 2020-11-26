#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <sys/time.h>

#define g -9.8
struct timeval tv;

struct System {
    double length;
    double M;
    double m;
    double x;
    double dx;
    double alpha;
    double dalpha;
    double Energy;
} sys;


double elappsed() {
	struct timeval now;
	gettimeofday(&now, 0);
	int ret = (now.tv_sec - tv.tv_sec) * 1000000
		+ now.tv_usec - tv.tv_usec;
	tv = now;
	return ret / 1.e6;
}

void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(0, w, h, 0, -1, 1);
}

void draw(double x,double alpha){

	/* screen position */
	double scale= 40;
	double sx = 320 + x*scale;
	double sy = 120;
	double length = sys.length*scale;

	glClear(GL_COLOR_BUFFER_BIT);

	/* cart */
	glBegin(GL_QUADS);
	glColor3f(1,1,1);
	glVertex2d(sx-1*scale, sy-0.5*scale);
	glVertex2d(sx+1*scale, sy-0.5*scale);
	glVertex2d(sx+1*scale, sy+0.5*scale);
	glVertex2d(sx-1*scale, sy+0.5*scale);
	glEnd();

	/* Rod */
	glColor3f(1,0,0);
	glBegin(GL_LINES);
	glVertex2d(sx, sy);
	glVertex2d(sx + length * sin(alpha), sy + length * cos(alpha));
	glEnd();

	// position
	/* glRasterPos2f(sx-1*scale+10, sy+0.5*scale); */
	/* char charray[200]; */
	/* sprintf(charray, "x=%2.2f", x); */
	/* glutBitmapString(GLUT_BITMAP_HELVETICA_18, charray); */

	/* glRasterPos2f(sx-1*scale+10, sy+0.5*scale-20); */
	/* sprintf(charray, "a=%2.2f", alpha); */
	/* glutBitmapString(GLUT_BITMAP_HELVETICA_18, charray); */

	glFlush();

}

void physics(double dt){

	// euler may suffice
	// simple pendulum
	/* double cosA=cos(sys.alpha); */
	/* double sinA=sin(sys.alpha); */
	/* double ddalpha = -g / sys.length * sinA; */
	/* sys.dalpha += ddalpha*dt; */
	/* sys.alpha += sys.dalpha*dt; */

	/* cart & pendulum */
	double cA = cos(sys.alpha);
	double sA = sin(sys.alpha);
	double l = sys.length;
	double M = sys.M;
	double m = sys.m;
	double D = m*l*l*(M+m*(1-cA*cA));
	double sdt = sys.dalpha;
	double sdx = sys.dx;
	double d = 0;
	double u=0;
	double ddx = (1/D)*(-m*m*l*l*g*cA*sA + m*l*l*(m*l*sdt*sdt*sA - d *sdx))+m*l*l*(1/D)*u;
	sys.dx+=ddx*dt;
	sys.x+=sys.dx*dt;

	double ddalpha = (1/D)*((m+M)*m*g*l*sA - m*l*cA*(m*l*sdt*sdt*sA - d* sdx)) -m*l*cA*(1/D)*u;
	sys.dalpha += ddalpha*dt;
	sys.alpha += sys.dalpha*dt;

}


void update()
{
	resize(640, 320);
	double us = elappsed();
	physics(us);
	draw(sys.x,sys.alpha);
}

void init_gfx(int *c, char **v)
{
	glutInit(c, v);
	glutInitDisplayMode(GLUT_RGB);
	glutInitWindowSize(640, 320);
	glutIdleFunc(update);
	glutCreateWindow("Pendulum");
}

int main(int c, char **v)
{
	sys.length = 2;
    sys.M=5;
    sys.m=1;
	sys.x=0;
	sys.dx=0;
	sys.alpha = atan2(-1, -1);
    sys.dalpha = 0.0;

	gettimeofday(&tv, 0);
	init_gfx(&c, v);
	glutMainLoop();
	return 0;
}
