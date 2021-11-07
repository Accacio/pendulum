/* [[file:../Readme.org::*The Code][The Code:1]] */
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>
#include <poll.h>
#include <sys/inotify.h>
#include <curses.h>
#include <locale.h>
#include <libtcc.h>
#include <string.h>

/* The Code:1 ends here */

/* [[file:../Readme.org::*The Code][The Code:2]] */
#define g -9.8
#define PI 3.14
#define deg(x) x/180.*PI

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 *( EVENT_SIZE + 16 ) )

char preamble[] = "#include <tcclib.h>\n"
    "#include <math.h>\n"
    "#define PI 3.14\n"
    "#define g -9.8\n"
    "typedef struct{\n"
    "    double l,M,m,d;\n"
    "} Sys;\n"
    "typedef struct{\n"
    "double x, dx, a, da;\n"
    "} State;\n"
    "double control(State state,Sys sys,double u)\n"
    "{\n"
    ;
char postamble[] = "return u;\n"
    "}";

/* The Code:2 ends here */

/* [[file:../Readme.org::*The Code][The Code:3]] */
typedef struct{
    double l,M,m,d;
} Sys;

typedef struct{
    double x, dx, a, da;
} State;
/* The Code:3 ends here */

/* [[file:../Readme.org::*The Code][The Code:4]] */
double tim(struct timeval * t){
    struct timeval n;
    gettimeofday(&n,0);
    int r=(n.tv_sec-t->tv_sec)*1.e6+n.tv_usec-t->tv_usec;
    *t = n;
    return r / 1.e6;
}

/* The Code:4 ends here */

/* [[file:../Readme.org::*Drawing][Drawing:1]] */
draw(State state,Sys sys,double u){

    /* screen position */
    int mx, my;
    getmaxyx(stdscr,my,mx);

    double x=mx/4*state.x+mx/2;
    double y=my/2+2;
    double l=sys.l*my/4;
    double ca=cos(state.a);
    double sa=sin(state.a);
    erase();

    mvprintw(0,0,"x=%3.2f m",state.x);
    mvprintw(1,0,"ẋ=%3.2f m/s",state.dx);
    mvprintw(2,0,"a=%3.2f rad",state.a);
    mvprintw(3,0,"ȧ=%3.2f rad/s",state.da);
    mvprintw(4,0,"u=%3.2f ",u);

    mvprintw(0,mx-16,"← to nudge left");
    mvprintw(1,mx-16,"→ to nudge right");
    mvprintw(2,mx-16,"↵ to restart");



    /* ground */
    move((int) y+2,0);
    hline(ACS_HLINE, mx);
    for(double i=-2;i<2;i+=0.5){
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
    for(double i = 0.1;i<1;i+=0.01){
        double absfa=fabsf(state.a);
        mvprintw(floor(y+i*l*ca),floor(x+i*l*sa),"|");
       if(sa<0&&ca>0.5) mvprintw(floor(y+i*l*ca),floor(x+i*l*sa),"/");
       if(sa>0&&ca>0.5) mvprintw(floor(y+i*l*ca),floor(x+i*l*sa),"\\");
       if(fabsf(ca)<0.1) mvprintw(floor(y+i*l*ca),floor(x+i*l*sa),"-");
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
physics(State * state,Sys sys,double dt,double u) {

    /* cart & pendulum */
    double x=state->x;
    double dx=state->dx;
    double a=state->a;
    double da=state->da;

    double ca=cos(a);
    double sa=sin(a);
    double l=sys.l;
    double M=sys.M;
    double m=sys.m;
    double D=m*l*l*(M+m*(1-ca*ca));
    double d=sys.d;
/* Physics Simulation:1 ends here */

/* [[file:../Readme.org::*Physics Simulation][Physics Simulation:2]] */
    /* euler */
    double ddx=(1/D)*(-m*m*l*l*g*ca*sa+m*l*l*(m*l*da*da*sa-d*dx))+m*l*l*(1/D)*u;
    state->dx+=ddx*dt;
    state->x+=state->dx*dt;

    double dda = (1/D)*((m+M)*m*g*l*sa-m*l*ca*(m*l*da*da*sa-d*dx))-m*l*ca*(1/D)*u;
    state->da+=dda*dt;
    state->a+=state->da*dt;
/* Physics Simulation:2 ends here */

/* [[file:../Readme.org::*Physics Simulation][Physics Simulation:3]] */
}
/* Physics Simulation:3 ends here */


char* get_fileChars(char* source_file){
    FILE *source;
    int fileSize;
    char *fileChars;

    source=fopen(source_file, "r");
    if(!source){
       exit(EXIT_FAILURE) ;
    }
    fseek(source,0,SEEK_END);
    fileSize=ftell(source);
    int newSize=sizeof(char)*sizeof(postamble)+sizeof(char)*sizeof(preamble)+sizeof(char)*fileSize+1;
    fileChars = (char*) malloc(newSize);
    memset(fileChars, '\0', newSize);
    strncpy(fileChars, preamble,newSize-1);
    fseek(source,0,SEEK_SET);
    fread((char*) fileChars+sizeof(preamble)-1,fileSize,1,source);
    strcat((char*)fileChars,postamble);
    fclose(source);
    return (char*) fileChars;
}

/* [[file:../Readme.org::*Main Loop][Main Loop:1]] */
int main(int c, char **v){
    /* lua_State* luaState = luaL_newstate(); */
    /* luaL_openlibs(luaState); */

    /* get default locale */
    char * locale = setlocale(LC_ALL, 0);
    setlocale(LC_ALL, "");

    TCCState *tccState;
    TCCState *tempTccState;


    int length, i = 0;
    int fd;
    char wdbuffer[BUF_LEN];
    fd = inotify_init1(IN_NONBLOCK);
    if ( fd <0)
    {
        fprintf(stderr,"inotify_init");
        exit(EXIT_FAILURE);
    }

    char source_file[10] = "control.c";

    /* Add watch */
    int control_watch = inotify_add_watch( fd, source_file, IN_MODIFY);
    if (control_watch==-1) {
        fprintf(stderr,"Could not add watch");
        exit(EXIT_FAILURE);
    }
    struct pollfd pfd = { fd, POLLIN, 0 };
    char *fileChars;

    double (*control)(State, Sys,double)= 0;
    double (*temp_control)(State, Sys,double)= 0;
    if(!control){
        setlocale(LC_ALL, locale);
        fileChars = get_fileChars(source_file);
        tccState = tcc_new();
        tcc_set_output_type(tccState, TCC_OUTPUT_MEMORY);
        if(tcc_compile_string(tccState, fileChars)<0){
            exit(EXIT_FAILURE);
        };
        free(fileChars);
        tcc_relocate(tccState, TCC_RELOCATE_AUTO);
        control = (double (*) (State,Sys,double)) tcc_get_symbol(tccState, "control");
        setlocale(LC_ALL, "");
    }



    uint8_t size = 4;
    Sys sys = {2,5,1,1}; // l M m d
    double sInit[4] = {-1.5, 0.0, 30.0/180*PI, 0.0}; // x dx α dα
    State state = {sInit[0],sInit[1],sInit[2],sInit[3]};

    struct timeval t;
    gettimeofday(&t, 0);

    initscr();
    curs_set(0);

    char ch;
    double u=0.0;
    if (nodelay(stdscr,TRUE)==ERR){
        return -1;
    }
    for(;;){
        /* Verify if control was changed */
        int ret = poll(&pfd,1,5);
        if (ret > 0)
        {
            length = read(fd, wdbuffer, sizeof wdbuffer);
            if (length>0)
            {
                struct inotify_event *watcher_event = ( struct inotify_event * ) &wdbuffer[ i ];
                if ( watcher_event->mask & IN_MODIFY )
                {
                    /* mvprintw(4, 3, "File changed"); */
                    if (watcher_event->wd==control_watch) {
                        setlocale(LC_ALL, locale);
                        fileChars = get_fileChars(source_file);
                        tempTccState = tcc_new();
                        tcc_set_output_type(tempTccState, TCC_OUTPUT_MEMORY);
                        if(tcc_compile_string(tempTccState, fileChars)<0){
                            tcc_delete(tempTccState);
                            control = (double (*) (State,Sys,double)) tcc_get_symbol(tccState, "control");
                            /* exit(EXIT_FAILURE); */
                        }
                        else
                        {
                            tcc_delete(tccState);
                            tcc_relocate(tempTccState, TCC_RELOCATE_AUTO);
                            control = (double (*) (State,Sys,double)) tcc_get_symbol(tempTccState, "control");
                            tccState = tempTccState;
                        }
                        setlocale(LC_ALL, "");

                    }
                }
            }
        }

        ch = getch();
        if (ch!=ERR){
            if(ch==68){state.dx-=1;} // nudge left
            if(ch==67){state.dx+=1;} // nudge right
            if(ch==65){
                u=0;
                sInit[0] =0;
                sInit[1] =0;
                sInit[2] =160./180*PI;
                sInit[3] =0;
            }
            if(ch==66){
                sInit[0] =-1.5;
                sInit[1] =0;
                sInit[2] =20.0/180*PI;
                sInit[3] =0;
            }
            if(ch==10){
                u=0;
                state.x =sInit[0];
                state.dx =sInit[1];
                state.a =sInit[2];
                state.da =sInit[3];
            } // Restart
            if(ch==113){
                break;
            }
        }
        u=control(state,sys,u);
        physics(&state,sys,tim(&t),u);
        draw(state,sys,u);
        usleep(20000);
    }
    tcc_delete(tccState);
    endwin();
    return 0;
}
/* Main Loop:1 ends here */
