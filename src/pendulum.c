#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <curses.h>

#define g -9.8
#define PI 3.14

typedef struct{
    double l,M,m,d;
} Sys;

double tim(struct timeval * t){
    struct timeval n;
    gettimeofday(&n,0);
    int r=(n.tv_sec-t->tv_sec)*1.e6+n.tv_usec-t->tv_usec;
    *t = n;
    return r / 1.e6;
}

int kbhit(void) {

struct timeval tv;
fd_set read_fd;
tv.tv_sec=0;
tv.tv_usec=0;
FD_ZERO(&read_fd);
FD_SET(0,&read_fd);
if(select(1, &read_fd, NULL, NULL, &tv) == -1)
return 0;
if(FD_ISSET(0,&read_fd))
return 1;
return 0;
}

draw(double * s,Sys sys,double u){

    /* screen position */
    int mx, _;
    getmaxyx(stdscr,_,mx);

    double x=mx/4*s[0]+mx/2;
    double y=30;
    double l=sys.l*10;
    double ca=cos(s[2]);
    double sa=sin(s[2]);
    clear();

    mvprintw(0,0,"x=%3.2f m",s[0]);
    mvprintw(1,0,".");
    mvprintw(2,0,"x=%3.2f m/s",s[1]);
    mvprintw(3,0,"α=%3.2f rad",s[2]);
    mvprintw(4,0,".");
    mvprintw(5,0,"α=%3.2f rad/s",s[3]);
    mvprintw(6,0,"u=%3.2f ",u);

    /* ground */
    move((int) y+2,0);
    hline(ACS_HLINE, mx);
    for(float i=-2;i<2;i+=0.5){
        mvprintw(y+3,mx/4*i+mx/2,"|",i);
        mvprintw(y+4,mx/4*i+mx/2,"%.2f",i);
    }

    /* cart */
    mvprintw(y-3,x-3," ----- ");
    mvprintw(y-2,x-3,"|     |");
    mvprintw(y-1,x-3,"|  M  |");
    mvprintw(y,x-3,"|     |");
    mvprintw(y+1,x-3," o---o ");

    /* rod */
    for(float i = 0.1;i<1;i+=0.01){
        mvprintw(floor(y+i*l*ca),floor(x+i*l*sa),"x");
    }
    y = floor(y+l*ca);
    x = floor(x+l*sa);
    mvprintw(y-2,x-2," --- ");
    mvprintw(y-1,x-2,"/   \\");
    mvprintw(y,x-2,"| m |");
    mvprintw(y+1,x-2,"\\   /");
    mvprintw(y+2,x-2," --- ");
    refresh();
}

physics(double * s,uint8_t size,Sys sys,double dt,double u) {

    /* cart & pendulum */
    double x=s[0];
    double dx=s[1];
    double a=s[2];
    double da=s[3];

    double ca=cos(a);
    double sa=sin(a);
    double l=sys.l;
    double M=sys.M;
    double m=sys.m;
    double D=m*l*l*(M+m*(1-ca*ca));
    double d=sys.d;

    /* double k[4] = { -10,-22,315,123}; */

    /* comment out to control manually */
    if(fabsf(a-PI)<0.6){
        /* control around upright linearization point */
        double k[4]={-100.0,-183.2,1683.0,646.6};
        u+=(-k[0])*s[0]+(-k[1])*s[1]+(-k[2])*(s[2]-PI)+(-k[3])*s[3];
    }
    else{
        /* swing up */
        double k[4]={20.0,0.0,-10,-10};
        double Wr=2*m*g*l;
        double W=m*l*l/2*da*da/2+m*g*l*(ca+1);
        u+=2*(W-Wr)*(da*ca>0?-1:1);
    }

    /* euler */
    double ddx=(1/D)*(-m*m*l*l*g*ca*sa+m*l*l*(m*l*da*da*sa-d*dx))+m*l*l*(1/D)*u;
    s[1]+=ddx*dt;
    s[0]+=s[1]*dt;

    double dda = (1/D)*((m+M)*m*g*l*sa-m*l*ca*(m*l*da*da*sa-d*dx))-m*l*ca*(1/D)*u;
    s[3]+=dda*dt;
    s[2]+=s[3]*dt;

}

main(int c, char **v){
    uint8_t size = 4;
    Sys sys = {2,5,1,1}; // l M m d
    double sInit[4] = {-1.5, 0.0, 30.0/180*PI, 0.0}; // x dx α dα
    double s[4] = {sInit[0],sInit[1],sInit[2],sInit[3]};

    struct timeval t;
    gettimeofday(&t, 0);

    initscr();
    curs_set(0);

    char ch;
    double u=0.0;
    for(;;){
        if (kbhit()){
            ch = getchar();
            if(ch==68){u=-20;} // nudge left
            if(ch==67){u=+20;} // nudge right
            if(ch==13){
                u=0;
                s[0] =sInit[0];
                s[1] =sInit[1];
                s[2] =sInit[2];
                s[3] =sInit[3];
            } // Restart
        }
        u*=0.9;
        physics(s,size,sys,tim(&t),u);
        draw(s,sys,u);
        usleep(20000);
    }
}
