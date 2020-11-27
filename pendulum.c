#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <curses.h>

/* #define g -9.8 */
#define g -10

typedef struct {
	double l;
	double M;
	double m;
	double d;
} System;

typedef struct {
	double x;
	double dx;
	double alpha;
	double dalpha;
} State;

double elappsed(struct timeval * tv) {
	struct timeval now;
	gettimeofday(&now, 0);
	int ret = (now.tv_sec - tv->tv_sec) * 1000000
		+ now.tv_usec - tv->tv_usec;
	*tv = now;
	return ret / 1.e6;
}

void draw(State state, System sys){

	/* screen position */
	int max_x, max_y;
	getmaxyx(stdscr, max_y, max_x);

	double sx = max_x/4*state.x+max_x/2;
	double sy = 30;
	double length = sys.l*10;

	clear();

	mvprintw(0,0, "x=%3.3f m",state.x);
	mvprintw(1,0, ".");
	mvprintw(2,0, "x=%3.3f m/s",state.dx);
	mvprintw(3,0, "α=%3.3f rad",state.alpha);
	mvprintw(4,0, ".");
	mvprintw(5,0, "α=%3.3f rad/s",state.dalpha);

	/* ground */
	move((int) sy+2,0);
	hline(ACS_HLINE, max_x);
	for(float i = -2;i<2;i+=0.5){
		mvprintw(sy+3,max_x/4*i+max_x/2,"|",i);
		mvprintw(sy+4,max_x/4*i+max_x/2,"%.2f",i);
	}


	/* cart */
	mvprintw((int) sy-3,(int) sx-3, " ----- ");
	mvprintw((int) sy-2,(int) sx-3, "|     |");
	mvprintw((int) sy-1,(int) sx-3, "|  M  |");
	mvprintw((int) sy  ,(int) sx-3, "|     |");
	mvprintw((int) sy+1,(int) sx-3, " o---o ");

	/* rod */
	for(float i = 0.1;i<1;i+=0.01){
		mvprintw(floor(sy +  i*length * cos(state.alpha)),floor(sx + i*length * sin(state.alpha)),"x");
	}

	mvprintw(-2+floor(sy +  length * cos(state.alpha)),floor(sx + length * sin(state.alpha))-2, " --- ");
	mvprintw(-1+floor(sy +  length * cos(state.alpha)),floor(sx + length * sin(state.alpha))-2, "/   \\");
	mvprintw(   floor(sy +  length * cos(state.alpha)),floor(sx + length * sin(state.alpha))-2, "| m |");
	mvprintw( 1+floor(sy +  length * cos(state.alpha)),floor(sx + length * sin(state.alpha))-2, "\\   /");
	mvprintw( 2+floor(sy +  length * cos(state.alpha)),floor(sx + length * sin(state.alpha))-2, " --- ");
	refresh();
}

void physics(State *state,System *sys,double dt,double u){

	/* cart & pendulum */
	double cA = cos(state->alpha);
	double sA = sin(state->alpha);
	double l = sys->l;
	double M = sys->M;
	double m = sys->m;
	double D = m*l*l*(M+m*(1-cA*cA));
	double sda = state->dalpha;
	double sdx = state->dx;
	double d = sys->d;

	/* double k[4] = { -10,-22,315,123}; */
	if(abs(state->alpha-3.14)<0.6){
		double k[4] = {-100.000,-183.179,1683.180,646.613};
		u += (-k[0])*state->x + (-k[1])*state->dx + (-k[2])*(state->alpha-3.14) + (-k[3])*state->dalpha;
	}
	else {
		/* swing up */
		double k[4] = {20.000,0.0,-10,-10};
		double Wref=2*m*g*l;
		double W=m*l*l/2*sda*sda/2+m*g*l*(cA+1);
		double umax=1;
		u += umax*(W-Wref)*(sda*cA>0?-1:1);
	}

	double ddx = (1/D)*(-m*m*l*l*g*cA*sA + m*l*l*(m*l*sda*sda*sA - d *sdx))+m*l*l*(1/D)*u;
	state->dx+=ddx*dt;
	state->x+=state->dx*dt;

	double ddalpha = (1/D)*((m+M)*m*g*l*sA - m*l*cA*(m*l*sda*sda*sA - d* sdx)) -m*l*cA*(1/D)*u;
	state->dalpha += ddalpha*dt;
	state->alpha += state->dalpha*dt;
}

int main(int c, char **v)
{

	State state;
	System sys;

	sys.l = 2;
	sys.M = 5;
	sys.m = 1;
	sys.d = 1;
	state.x = -1.5;
	state.dx = 0;
	state.alpha = 30.0/180*3.14;
	/* state.alpha = 3.14-0.5; */
	state.dalpha = 0.0;
	struct timeval tv;
	gettimeofday(&tv, 0);

	initscr();
	curs_set(FALSE);

	for(;;){
		double us = elappsed(&tv);
		physics(&state,&sys,us,0);

		draw(state,sys);
        usleep(2000);
	}
	return 0;
}
