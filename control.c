double x=state.x;
double dx=state.dx;
double a=state.a;
double da=state.da;

double ca=cos(a);
double sa=sin(a);
double l=sys.l;
double M=sys.M;
double m=sys.m;
double D=m*l*l*(M+m*(1-ca*ca));
double d=sys.d;
/* x=x+sin(t/2); */

if(fabsf(a-PI)<0.9){
    double k[4]={-100.0,-183.2,1683.0,646.6};
    u=(-k[0])*x+(-k[1])*dx+(-k[2])*(a-PI)+(-k[3])*da;
}
else{
    double k[4]={20.0,0.0,-10,-10};
    double Wr=2*m*g*l;
    double W=m*l*l/2*da*da/2+m*g*l*(ca+1);
    u=2.*(W-Wr)*(da*ca>0?-1:1);
}
/* u=0; */
