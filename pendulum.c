#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <curses.h>

#define g -9.8
#define PI 3.14

typedef struct {
	double l, M, m, d;
} System;

double elappsed(struct timeval * t) {
	struct timeval now;
	gettimeofday(&now, 0);
	int ret = (now.tv_sec - t->tv_sec) * 1.e6
		+ now.tv_usec - t->tv_usec;
	*t = now;
	return ret / 1.e6;
}

void draw(double * s,System sys){

	/* screen position */
	int m_x, _;
	getmaxyx(stdscr, _, m_x);

	double sx = m_x/4*s[0]+m_x/2;
	double sy = 30;
	double l = sys.l*10;
	double ca = cos(s[2]);
	double sa = sin(s[2]);
	clear();

	mvprintw(0,0, "x=%3.2f m",s[0]);
	mvprintw(1,0, ".");
	mvprintw(2,0, "x=%3.2f m/s",s[1]);
	mvprintw(3,0, "α=%3.2f rad",s[2]);
	mvprintw(4,0, ".");
	mvprintw(5,0, "α=%3.2f rad/s",s[3]);

	/* ground */
	move((int) sy+2,0);
	hline(ACS_HLINE, m_x);
	for(float i = -2;i<2;i+=0.5){
		mvprintw(sy+3,m_x/4*i+m_x/2,"|",i);
		mvprintw(sy+4,m_x/4*i+m_x/2,"%.2f",i);
	}

	/* cart */
	mvprintw( sy-3, sx-3, " ----- ");
	mvprintw( sy-2, sx-3, "|     |");
	mvprintw( sy-1, sx-3, "|  M  |");
	mvprintw( sy  , sx-3, "|     |");
	mvprintw( sy+1, sx-3, " o---o ");

	/* rod */
	for(float i = 0.1;i<1;i+=0.01){
		mvprintw(floor(sy +  i*l * ca),floor(sx + i*l * sa),"x");
	}
	sy = floor(sy +  l * ca);
	sx = floor(sx + l * sa);
	mvprintw(sy -2,sx-2, " --- ");
	mvprintw(sy -1,sx-2, "/   \\");
	mvprintw(sy   ,sx-2, "| m |");
	mvprintw(sy +1,sx-2, "\\   /");
	mvprintw(sy +2,sx-2, " --- ");
	refresh();
}

void physics(double * s,uint8_t sSize,System sys,double dt,double u){

	/* cart & pendulum */
	double x = s[0];
	double dx = s[1];
	double a = s[2];
	double da = s[3];

	double ca = cos(a);
	double sa = sin(a);
	double l = sys.l;
	double M = sys.M;
	double m = sys.m;
	double D = m*l*l*(M+m*(1-ca*ca));
	double d = sys.d;
	/* double k[4] = { -10,-22,315,123}; */

	if(fabsf(a-PI)<0.6){
		double k[4] = {-100.0,-183.2,1683.0,646.6};
		u += (-k[0])*s[0] + (-k[1])*s[1] + (-k[2])*(s[2]-PI) + (-k[3])*s[3];
	}
	else {
		/* swing up */
		double k[4] = {20.0,0.0,-10,-10};
		double Wr=2*m*g*l;
		double W=m*l*l/2*da*da/2+m*g*l*(ca+1);
		u += 2*(W-Wr)*(da*ca>0?-1:1);
	}

	/* euler */
	double ddx = (1/D)*(-m*m*l*l*g*ca*sa + m*l*l*(m*l*da*da*sa - d *dx))+m*l*l*(1/D)*u;
	s[1]+=ddx*dt;
	s[0]+=s[1]*dt;

	double ddalpha = (1/D)*((m+M)*m*g*l*sa - m*l*ca*(m*l*da*da*sa - d* dx)) -m*l*ca*(1/D)*u;
	s[3]+=ddalpha*dt;
	s[2]+=s[3]*dt;

}

int main(int c, char **v)
{
	uint8_t sSize = 4;
	System sys = {2,5,1,1}; // l M m d
	double s[4] = {-1.5, 0.0, 30.0/180*PI, 0.0}; // x dx α dα

	/* state.alpha = PI-0.5; */
	struct timeval t;
	gettimeofday(&t, 0);

	initscr();
	curs_set(FALSE);

	for(;;){
		double us = elappsed(&t);
		physics(s,sSize,sys,us,0);
		draw(s,sys);
        usleep(20000);
	}
	return 0;
}
