#include <iostream>
#include <fstream>
#include <math.h>
#include <cstring>
#include <sys/time.h>

using namespace std;

struct allData { 
double** vi;
double** vr;
const double pi_over_nx = M_PI/double(nx);

int i, j, t;

static constexpr double dividebyfour = 0.25;
static constexpr double dividebytwo = 0.5;
static constexpr int nx = 10000;
static constexpr int ny = 200;
static constexpr int nt = 200;

allData() {
        vi = new double*[nx];
        vr = new double*[nx];
        for(i=0;i<nx;++i) {
                vi[i]=new double[ny];                                    
                vr[i]=new double[ny];
        }
}

~allData() {
        for(i=0;i<nx;++i) {
                delete[] vi[i];
                delete[] vr[i];
        }
        delete[] vi;
        delete[] vr;
}

};

struct timeval start, endTime;

int main(int argc, char* argv[]) {

        gettimeofday(&start, NULL);
        allData data;

        for(data.i=0;data.i<data.nx;++data.i) {
                const double constant(double(data.i*data.i)*sin(data.pi_over_nx*double(data.i)));
                for(data.j=0;data.j<data.ny;++data.j) {
                        data.vi[data.i][data.j]= constant * double(data.j);         
                        data.vr[data.i][data.j] = 0;
                }
        }

        ofstream fout("data_out");
                
        for(data.t=0;data.t<data.nt;++data.t) {                                               
                                                                                
                cout<<"\n"<<data.t;cout.flush();                                          
                                                                                
                for(data.i=1;data.i<data.nx-1;++data.i) {
                        data.vr[data.i][0]=(data.vi[data.i+1][0]+data.vi[data.i-1][0]+15.45+data.vi[data.i][1])*data.dividebyfour;
                        data.vr[data.i][199]=(data.vi[data.i+1][199]+data.vi[data.i-1][199]+data.vi[data.i][198]-6.7)*data.dividebyfour;
                }
                for(data.i=1;data.i<data.nx-1;++data.i) {
                        for(data.j=1;data.j<data.ny-1;data.j++) {
                                data.vr[data.i][data.j]=(data.vi[data.i+1][data.j]+data.vi[data.i-1][data.j]+data.vi[data.i][data.j-1]+data.vi[data.i][data.j+1])*data.dividebyfour;
                        }
                }
                for(data.j=1;data.j<data.ny-1;++data.j) {
                        data.vr[0][data.j]=(data.vi[1][data.j]+10.+data.vi[0][data.j-1]+data.vi[0][data.j+1])*data.dividebyfour;
                        data.vr[9999][data.j]=(5.+data.vi[9998][data.j]+data.vi[9999][data.j-1]+data.vi[9999][data.j+1])*data.dividebyfour;
                }
                
                for(data.i=0;data.i<data.nx;++data.i) {

                        for(data.j=0;data.j<data.ny;++data.j) {                                 
                                const double valvr(fabs(data.vr[data.i][data.j]));
                                const double valvi(fabs(data.vi[data.i][data.j]));
                                if(fabs(valvr-valvi)<1e-2) fout<<"\n"<<data.t<<" "<<data.i<<" "<<data.j<<" "<<valvi<<" "<<valvr; 
                                data.vi[data.i][data.j]=(data.vi[data.i][data.j]*data.dividebytwo)+(data.vr[data.i][data.j]*data.dividebytwo);
                        }
                }
        }
        gettimeofday(&endTime, NULL);
        cout << "\nTime taken = " << ((endTime.tv_sec - start.tv_sec) + (endTime.tv_usec - start.tv_usec) / 1e6) << " seconds";
}

