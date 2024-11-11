#include <iostream>
#include <fstream>
#include <math.h>
#include <sstream>
#include <sys/time.h>
#include <cstring>
#include <algorithm>

using namespace std;


struct baseArgs{
        static constexpr int nx = 10000;                                            // Changed const, could use a typedef here?
        static constexpr int ny = 200;                                              // Not sure if I like this 
        static constexpr int nt = 200;                                              // Changed to 200 to allow for more testing
};

struct countersAndScalers {
        static constexpr double dividebyfour = 0.25;
        static constexpr double dividebytwo = 0.5;
        int i, j, t;
};

struct array2D {
    double** vi = new double*[baseArgs::nx];
    double** vr = new double*[baseArgs::nx];
};

int main(int argc, char* argv[]) {

        baseArgs args;
        array2D arrays;
        countersAndScalers cs;

        const double pi= M_PI;
        for(cs.i=0;cs.i<args.nx;++cs.i) {
                arrays.vi[cs.i]=new double[args.ny];                                    // Creating the 2D array here
                arrays.vr[cs.i]=new double[args.ny];
        }
        
        const double pi_over_nx = pi/double(args.nx);     
          
        for(cs.i=0;cs.i<args.nx;++cs.i) {
                const double constant = double(cs.i*cs.i)*sin(pi_over_nx*double(cs.i));
                for(cs.j=0;cs.j<args.ny;++cs.j) {

                        arrays.vi[cs.i][cs.j]=constant * double(cs.j);         
                        memset(&arrays.vr[cs.i][cs.j],0,sizeof(double));                // Ah why not eh? Seems to perform identically to the line below so I'm keeping it
                                                                                        // vr[i][j] = 0;           // Testing against memset
                }
        }
        
                                                                                        // memset(vr,0,nx*ny*sizeof(double));           Not possible as we are using a 2D array here, no guarantee that the memory is contiguous          
                                                                                        // Could look to sort the data here to demostrate awareness of branch prediction

        ofstream fout("data_out");
                
        for(cs.t=0;cs.t<args.nt;++cs.t) {                                               // Will try and test against zero here as it is not used for array access, so we will not be interfering with sequential cache access, probably not allowed as it changes the logic of the code
                                                                                        // Will try and make small modification to permit the above modification without changing the output of the code
                cout<<"\n"<<cs.t;cout.flush();                                          // Changed the logic to (nt-t) to allow for expected output, doesnt seem to increase runtime
                                                                                        // Adds new lines to the output data, abandoned this idea as its not worth the effort
                for(cs.i=1;cs.i<args.nx-1;cs.i++) {
                        arrays.vr[cs.i][0]=(arrays.vi[cs.i+1][0]+arrays.vi[cs.i-1][0]+15.45+arrays.vi[cs.i][1])*cs.dividebyfour;
                        arrays.vr[cs.i][199]=(arrays.vi[cs.i+1][199]+arrays.vi[cs.i-1][199]+arrays.vi[cs.i][198]-6.7)*cs.dividebyfour;
                }
                for(cs.i=1;cs.i<args.nx-1;cs.i++) {
                        for(cs.j=1;cs.j<args.ny-1;cs.j++) {
                                arrays.vr[cs.i][cs.j]=(arrays.vi[cs.i+1][cs.j]+arrays.vi[cs.i-1][cs.j]+arrays.vi[cs.i][cs.j-1]+arrays.vi[cs.i][cs.j+1])*cs.dividebyfour;
                        }
                }
                for(cs.j=1;cs.j<args.ny-1;cs.j++) {
                        arrays.vr[0][cs.j]=(arrays.vi[1][cs.j]+10.+arrays.vi[0][cs.j-1]+arrays.vi[0][cs.j+1])*cs.dividebyfour;
                        arrays.vr[9999][cs.j]=(5.+arrays.vi[9998][cs.j]+arrays.vi[9999][cs.j-1]+arrays.vi[9999][cs.j+1])*cs.dividebyfour;
                }
                
                for(cs.i=0;cs.i<args.nx;++cs.i) {

                        for(cs.j=0;cs.j<args.ny;++cs.j) {                               // Tried loop unrolling but the duplication of if statements kills the runtime
                                                                                        // Calculating the absolute values slowed it by almsot a second
                                const double valvr(fabs(arrays.vr[cs.i][cs.j]));
                                const double valvi(fabs(arrays.vi[cs.i][cs.j]));
                                if(fabs(valvr-valvi)<1e-2) fout<<"\n"<<cs.t<<" "<<cs.i<<" "<<cs.j<<" "<<valvi<<" "<<valvr; 
                                arrays.vi[cs.i][cs.j]=(arrays.vi[cs.i][cs.j]*cs.dividebytwo)+(arrays.vr[cs.i][cs.j]*cs.dividebytwo);
                        }
                }
        }
}

