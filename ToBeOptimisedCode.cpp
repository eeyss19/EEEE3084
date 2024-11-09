#include <iostream>
#include <fstream>
#include <math.h>
#include <sstream>
#include <sys/time.h>
#include <cstring>

using namespace std;


int main(int argc, char* argv[]) {

        struct timeval start_time,end_time;

        const int nx(10000);            // Changed const, could use a typedef here?

        const int ny(200);

        const int nt(200);

        double** vi=new double*[nx];

        double** vr=new double*[nx];

        const double pi=(4.*atan(1.));

        for(int i=0;i<nx;++i) {

                vi[i]=new double[ny];           // Creating the 2D array here
                // std::memset(a,0.,ny*sizeof(double)); // Not applicable here
                vr[i]=new double[ny];

        }
        
        const double pi_over_nx = pi/double(nx);     
        // gettimeofday(&start_time,NULL);    
        for(int i=0;i<nx;++i) {

                for(int j=0;j<ny;++j) {

                        vi[i][j]=double(i*i)*double(j)*sin(pi_over_nx*double(i));

                        memset(&vr[i][j],0,sizeof(double));         // Ah why not eh? Seems to perform identically to the line below so I'm keeping it
                        // vr[i][j] = 0;           // Testing against memset
                }
        }
        gettimeofday(&end_time,NULL);
        // cout<<"\nInitialisation time (s) = "<<end_time.tv_sec-start_time.tv_sec+(end_time.tv_usec-start_time.tv_usec)/1e6;
        // memset(vr,0,nx*ny*sizeof(double));           Not possible as we are using a 2D array here, no guarantee that the memory is contiguous          
        // Could look to sort the data here to demostrate awareness of branch prediction

        ofstream fout("data_out");
        const double dividebyfour(0.25);
        const double dividebytwo(0.5);
        
        for(int t=0;t<nt;++t) {         // Will try and test against zero here as it is not used for array access, so we will not be interfering with sequential cache access, probably not allowed as it changes the logic of the code
                                        // Will try and make small modification to permit the above modification without changing the output of the code
                cout<<"\n"<<t;cout.flush();             // Changed the logic to (nt-t) to allow for expected output, doesnt seem to increase runtime
                                                        // Adds new lines to the output data, abandoned this idea as its not worth the effort
                for(int i=0;i<nx;++i) {

                        for(int j=0;j<ny;++j) {

                                if(i > 0 && i < nx-1 && j > 0 && j < ny-1) {            // The idea is that if one of these tests fails then we can isolate exactly which of the two conditions if failed.

                                        vr[i][j]=(vi[i+1][j]+vi[i-1][j]+vi[i][j-1]+vi[i][j+1])*dividebyfour;              // Tried to change i+1 to use increment operator but practically halts the runtime, why is that?

                                } else if(i == 0 && j > 0 && j < ny-1) {     // Redundant condition, could be removed to save time

                                        vr[i][j]=(vi[i+1][j]+10.+vi[i][j-1]+vi[i][j+1])*dividebyfour;                     // All of these division operations could be replaced with a bitshift operation, but the compiler will probably do this for us
                                                                                                                // Will try it anyway? Nevermind its a double, so it will not work
                                } else if(i == nx-1 && j > 0 && j < ny-1) {          // Again repeated conditional, why check if by being at this point we know the result of that check anyway                        // Yet still faster to multiply by 0.25, managed to get below 6 seconds here !

                                        vr[i][j]=(5.+vi[i-1][j]+vi[i][j-1]+vi[i][j+1])*dividebyfour;            // Would a switch statement be faster than if/else? 

                                } else if(j == 0 && i > 0 && i < nx-1) {

                                        vr[i][j]=(vi[i+1][j]+vi[i-1][j]+15.45+vi[i][j+1])*dividebyfour;

                                } else if(j == ny-1 && i > 0 && i < nx-1) {

                                        vr[i][j]=(vi[i+1][j]+vi[i-1][j]+vi[i][j-1]-6.7)*dividebyfour;

                                }
                        }
                }
                // Loop unrolling for these final two loops could be beneficial
                
                for(int i=0;i<nx;++i) {

                        for(int j=0;j<ny;++j) {           // Tried loop unrolling but the duplication of if statements kills the runtime

                                if(fabs(fabs(vr[i][j])-fabs(vi[i][j]))<1e-2) fout<<"\n"<<t<<" "<<i<<" "<<j<<" "<<fabs(vi[i][j])<<" "<<fabs(vr[i][j]); 
                        }
                }
                
                 
                for(int i=0;i<nx;++i) {              // Merged these two loops into one, seems to be noticably faster

                        for(int j=0;j<ny;j+=8) {        // Loop unrolling 200 % 8 = 0, so we can unroll. Also 8 doubles to a cache line so hopefully this will be faster
                        
                        //  vi[i][j]=(vi[i][j]*dividebytwo)+(vr[i][j]*dividebytwo);         

                        vi[i][j]=(vi[i][j]*dividebytwo)+(vr[i][j]*dividebytwo);         // Loop unrolling may be slightly faster, code may be slightly less readable but I think it shows awareness of cache performance
                        vi[i][j+1]=(vi[i][j+1]*dividebytwo)+(vr[i][j+1]*dividebytwo);
                        vi[i][j+2]=(vi[i][j+2]*dividebytwo)+(vr[i][j+2]*dividebytwo);
                        vi[i][j+3]=(vi[i][j+3]*dividebytwo)+(vr[i][j+3]*dividebytwo);
                        vi[i][j+4]=(vi[i][j+4]*dividebytwo)+(vr[i][j+4]*dividebytwo);
                        vi[i][j+5]=(vi[i][j+5]*dividebytwo)+(vr[i][j+5]*dividebytwo);
                        vi[i][j+6]=(vi[i][j+6]*dividebytwo)+(vr[i][j+6]*dividebytwo);
                        vi[i][j+7]=(vi[i][j+7]*dividebytwo)+(vr[i][j+7]*dividebytwo);

                        }
                }
        }
}

