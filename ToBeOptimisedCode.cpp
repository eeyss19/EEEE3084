#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;

int main(int argc, char* argv[]) {

        int nx(10000);

        int ny(200);

        int nt(200);

        double** vi=new double*[nx];

        double** vr=new double*[nx];

        double pi=(4.*atan(1.));

        for(int i=0;i<nx;i++) {

                vi[i]=new double[ny];

                vr[i]=new double[ny];

        }

        for(int i=0;i<nx;i++) {

                for(int j=0;j<ny;j++) {

                        vi[i][j]=double(i*i)*double(j)*sin(pi/double(nx)*double(i));

                        vr[i][j]=0.;
                }
        }


        ofstream fout("data_out");

        for(int t=0;t<nt;t++) {

                cout<<"\n"<<t;cout.flush();

                for(int i=0;i<nx;i++) {

                        for(int j=0;j<ny;j++) {

                                if(i>0&&i<nx-1&&j>0&&j<ny-1) {

                                        vr[i][j]=(vi[i+1][j]+vi[i-1][j]+vi[i][j-1]+vi[i][j+1])/4.;

                                } else if(i==0&&i<nx-1&&j>0&&j<ny-1) {

                                        vr[i][j]=(vi[i+1][j]+10.+vi[i][j-1]+vi[i][j+1])/4.;

                                } else if(i>0&&i==nx-1&&j>0&&j<ny-1) {

                                        vr[i][j]=(5.+vi[i-1][j]+vi[i][j-1]+vi[i][j+1])/4.;

                                } else if(i>0&&i<nx-1&&j==0&&j<ny-1) {

                                        vr[i][j]=(vi[i+1][j]+vi[i-1][j]+15.45+vi[i][j+1])/4.;

                                } else if(i>0&&i<nx-1&&j>0&&j==ny-1) {

                                        vr[i][j]=(vi[i+1][j]+vi[i-1][j]+vi[i][j-1]-6.7)/4.;

                                }
                        }
                }

                for(int i=0;i<nx;i++) {

                        for(int j=0;j<ny;j++) {

                                if(fabs(fabs(vr[i][j])-fabs(vi[i][j]))<1e-2) fout<<"\n"<<t<<" "<<i<<" "<<j<<" "<<fabs(vi[i][j])<<" "<<fabs(vr[i][j]);

                        }
                }

                for(int i=0;i<nx;i++) {

                        for(int j=0;j<ny;j++) vi[i][j]=vi[i][j]/2.+vr[i][j]/2.;

                }
        }
}

