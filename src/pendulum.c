/* [[file:../Readme.org::*The Code][The Code:1]] */
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <curses.h>
#include <locale.h>
#include <libtcc.h>

char my_program[] =
    "#include <tcclib.h>\n"
    "#include <math.h>\n"
    "#define PI 3.14\n"
    "#define g -9.8\n"
    "typedef struct{"
    "    double l,M,m,d;"
    "} Sys;"
    ""
    "double control(double * s,uint8_t size,Sys sys,double u)"
    "{"
    "double x=s[0];"
    "double dx=s[1];"
    "double a=s[2];"
    "double da=s[3];"
    ""
    "double ca=cos(a);"
    "double sa=sin(a);"
    "double l=sys.l;"
    "double M=sys.M;"
    "double m=sys.m;"
    "double D=m*l*l*(M+m*(1-ca*ca));"
    "double d=sys.d;"
    "if(fabsf(a-PI)<0.6){"
    "double k[4]={-100.0,-183.2,1683.0,646.6};"
    "u=(-k[0])*s[0]+(-k[1])*s[1]+(-k[2])*(s[2]-PI)+(-k[3])*s[3];"
    "}"
    "else{"
    "double k[4]={20.0,0.0,-10,-10};"
    "double Wr=2*m*g*l;"
    "double W=m*l*l/2*da*da/2+m*g*l*(ca+1);"
    "u=2*(W-Wr)*(da*ca>0?-1:1);"
    "}"
    "return u;"
    "}";
/* The Code:1 ends here */

/* [[file:../Readme.org::*The Code][The Code:2]] */
#define g -9.8
#define PI 3.14
/* The Code:2 ends here */

/* [[file:../Readme.org::*The Code][The Code:3]] */
typedef struct{
    double l,M,m,d;
} Sys;
/* The Code:3 ends here */

/* [[file:../Readme.org::*The Code][The Code:4]] */
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
/* The Code:4 ends here */

/* [[file:../Readme.org::*Drawing][Drawing:1]] */
draw(double * s,Sys sys,double u){

    /* screen position */
    int mx, my;
    getmaxyx(stdscr,my,mx);

    double x=mx/4*s[0]+mx/2;
    double y=my/2+2;
    double l=sys.l*my/4;
    double ca=cos(s[2]);
    double sa=sin(s[2]);
    clear();

    mvprintw(0,0,"x=%3.2f m",s[0]);
    mvprintw(1,0,"ẋ=%3.2f m/s",s[1]);
    mvprintw(2,0,"a=%3.2f rad",s[2]);
    mvprintw(3,0,"ȧ=%3.2f rad/s",s[3]);
    mvprintw(4,0,"u=%3.2f ",u);

    mvprintw(0,mx-16,"← to nudge left");
    mvprintw(1,mx-16,"→ to nudge right");
    mvprintw(2,mx-16,"⮠ to restart");



    /* ground */
    move((int) y+2,0);
    hline(ACS_HLINE, mx);
    for(float i=-2;i<2;i+=0.5){
        mvprintw(y+3,mx/4*i+mx/2,"|",i);
        mvprintw(y+4,mx/4*i+mx/2,"%.2f",i);
    }

    /* cart */
    mvprintw(y-3,x-4,"┌───────┐");
    mvprintw(y-2,x-4,"|       │");
    mvprintw(y-1,x-4,"|   M   │");
    mvprintw(y,x-4  ,"|       │");
    mvprintw(y+1,x-4,"└o-----o┘");

    /* rod */
    for(float i = 0.1;i<1;i+=0.01){
        mvprintw(floor(y+i*l*ca),floor(x+i*l*sa),"│");
    }
    y = floor(y+l*ca);
    x = floor(x+l*sa);
    mvprintw(y-2,x-2,"");
    mvprintw(y-1,x-2,"┌───┐");
    mvprintw(y,x-2  ,"| m |");
    mvprintw(y+1,x-2,"└───┘");
    mvprintw(y+2,x-2,"");
    refresh();
}
/* Drawing:1 ends here */

/* [[file:../Readme.org::*Physics Simulation][Physics Simulation:1]] */
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
    /* if(fabsf(a-PI)<0.6){ */
    /*     /\* control around upright linearization point *\/ */
    /*     double k[4]={-100.0,-183.2,1683.0,646.6}; */
    /*     u+=(-k[0])*s[0]+(-k[1])*s[1]+(-k[2])*(s[2]-PI)+(-k[3])*s[3]; */
    /* } */
    /* else{ */
    /*     /\* swing up *\/ */
    /*     double k[4]={20.0,0.0,-10,-10}; */
    /*     double Wr=2*m*g*l; */
    /*     double W=m*l*l/2*da*da/2+m*g*l*(ca+1); */
    /*     u+=2*(W-Wr)*(da*ca>0?-1:1); */
    /* } */
/* Physics Simulation:1 ends here */

/* [[file:../Readme.org::*Physics Simulation][Physics Simulation:2]] */
    /* euler */
    double ddx=(1/D)*(-m*m*l*l*g*ca*sa+m*l*l*(m*l*da*da*sa-d*dx))+m*l*l*(1/D)*u;
    s[1]+=ddx*dt;
    s[0]+=s[1]*dt;

    double dda = (1/D)*((m+M)*m*g*l*sa-m*l*ca*(m*l*da*da*sa-d*dx))-m*l*ca*(1/D)*u;
    s[3]+=dda*dt;
    s[2]+=s[3]*dt;
/* Physics Simulation:2 ends here */

/* [[file:../Readme.org::*Physics Simulation][Physics Simulation:3]] */
}
/* Physics Simulation:3 ends here */

/* [[file:../Readme.org::*Main Loop][Main Loop:1]] */
main(int c, char **v){
    TCCState *tccState;
    char source_file[10] = "control.c";
    FILE *source;
    int fileSize;
    char *fileChars;

    source=fopen(source_file, "r");
    if(!source){
       exit(EXIT_FAILURE) ;
    }
    fseek(source,0,SEEK_END);
    fileSize=ftell(source);
    fileChars = (char*) realloc(fileChars,sizeof(char)*fileSize+1);
    fseek(source,0,SEEK_SET);
    fread((char*) fileChars,fileSize,1,source);
    fclose(source);


    tccState = tcc_new();
    tcc_set_output_type(tccState, TCC_OUTPUT_MEMORY);

    double (*control)(double *,uint8_t,Sys,double)= 0;

    if(!control){
        if(tcc_compile_string(tccState, fileChars)>0){
            exit(EXIT_FAILURE);
        };
        tcc_relocate(tccState, TCC_RELOCATE_AUTO);
        control = tcc_get_symbol(tccState, "control");
    }


    setlocale(LC_ALL, "");

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
            if(ch==68){s[2]+=30./180*PI;} // nudge left
            if(ch==67){s[2]-=30./180*PI;} // nudge right
            if(ch==65){
                sInit[0] =0;
                sInit[1] =0;
                sInit[2] =160./180*PI;
                sInit[3] =0;
            }
            if(ch==66){
                sInit[0] =0;
                sInit[1] =0;
                sInit[2] =30.0/180*PI;
                sInit[3] =0;
            }
            if(ch==13){
                u=0;
                s[0] =sInit[0];
                s[1] =sInit[1];
                s[2] =sInit[2];
                s[3] =sInit[3];
            } // Restart
            if(ch==27){
                break;
            }
        }
        u=control(s,size,sys,u);
        physics(s,size,sys,tim(&t),u);
        draw(s,sys,u);
        usleep(20000);
    }
    tcc_delete(tccState);
    endwin();
    return 0;
}
/* Main Loop:1 ends here */
