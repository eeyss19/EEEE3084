#include <iostream>
#include <sys/time.h>
#include <omp.h>
#include <math.h>
#include <fstream>

struct timeval start_time, end_time;

using namespace std;



double i_dependent_fn(int i){       // Dependent on i to simulate operations not equal in effort, encourages the use of dynamic scheduling
    double result = .0;
    for(int j = 0 ; j < (i % 1000) * 10 ; ++j){
        result += sin(j) * cos(j);
    }
    return result;
}

int main() {
    std::ofstream outfile("task_two_output.csv");
    // const int max_offset = 1024;
    const int max_n(2000000);
    const int offset(0);
    const int num_repeats(10); 

    double total_time_static(0.0);
    // double total_time_dynamic(0.0);

    // omp_set_num_threads(6);

    int n, i, repeat;
    
    for (n = 2; n <= max_n; n *= 2) { 
    // for(int offset = 1; offset <= max_offset; offset *= 2) {
        double *a = new double[n];
        double *b = new double[n];
        double *c = new double[n + offset];

        for (i = 0; i < n; i++) {
            b[i] = c[i] = i;
        }

        gettimeofday(&start_time, NULL); 
        // #pragma omp parallel for schedule(static) private(i, repeat) default(shared)
        for (repeat = 0 ; repeat < num_repeats; repeat++) {
                // #pragma omp parallel for schedule(static) private(i)
                for (i = 0; i < (n - offset); i++) {
                    a[i] = b[i] + c[i + offset];
                    // a[i] = b[i] + c[i + offset] + i_dependent_fn(i); 
                    }
                }

        gettimeofday(&end_time, NULL); 

        double elapsed_time_static = (end_time.tv_sec - start_time.tv_sec) +            // Time for the nested loops, for increased accuracy
                                     (end_time.tv_usec - start_time.tv_usec) / 1e6;
        double time_per_iteration_static = elapsed_time_static / num_repeats;           // Normalised time per iteration
        cout<< "Static scheduling: for n = " << n << " and offset = " << offset <<", average wall time = " << time_per_iteration_static << " seconds" << endl;
        total_time_static += time_per_iteration_static;

        // n is the amount of doubles per array, 8 bytes per double, n->kB = n * 8 / 1024
        double n_kB = n * 8 / 1024;
        outfile << n_kB << "," << time_per_iteration_static << endl;


        // gettimeofday(&start_time, NULL);
        // for (int repeat = 0 ; repeat < num_repeats; repeat++) {
        //         #pragma omp parallel for schedule(dynamic, 100) private(i)
        //         for (i = 0; i < (n - offset); i++) {
        //             // a[i] = b[i] + c[i + offset];
        //             a[i] = b[i] + c[i + offset] + i_dependent_fn(i); 
        //             }

        //         }
        // gettimeofday(&end_time, NULL); 

        // double elapsed_time_dynamic = (end_time.tv_sec - start_time.tv_sec) +
        //                               (end_time.tv_usec - start_time.tv_usec) / 1e6;
        // double time_per_iteration_dynamic = elapsed_time_dynamic / num_repeats;
        // cout<< "Dynamic scheduling: for n = " << n << " and offset = " << offset <<", average wall time = " << elapsed_time_dynamic << " seconds" << endl;
        // total_time_dynamic += time_per_iteration_dynamic;

        delete[] a;
        delete[] b;
        delete[] c;
    }
    outfile.close();
    // cout<<"Total time with dynamic scheduling = " << total_time_dynamic << " seconds" << endl;
    // cout<<"Total time with static scheduling = " << total_time_static * 1000 << " milliseconds" << endl;

    return 0;
}
