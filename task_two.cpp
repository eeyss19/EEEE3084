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
    std::ofstream outfile("task_two_output_offset_2dStaticVsDynamic.csv");
    // outfile << "Offset,Runtime (s),Runtime Difference (s)\n";
    // outfile << "Memory (kB),Runtime (s),Runtime Difference (s)\n";
    outfile << "Memory (kB),Static Runtime (s),Dynamic Runtime (s)\n";
    const int max_offset = 10;
    // const int max_n(4*1024*1024);
    const int offset(0);
    const int num_repeats(1); // Changed from 10 -> 100

    double total_time_static(0.0);
    double total_time_dynamic(0.0);

    omp_set_num_threads(6);

    // const int n = 1216000;
    int n;
    int i, repeat;
    double last_time = 0.0;
    
    // for (n = 0; n <= 2000000; n += (n < 49152 ? 512 : n < 196608 ? 4096 : n < 1572864 ? 16384 : 65536)) {
    // #pragma omp parallel for private(n, i, repeat, start_time, end_time) reduction(+:total_time_static, total_time_dynamic) default(shared)
    for (n = 500 ; n < 2000000 ; n*=2) { 
    // for(int offset = 0; offset < max_offset; offset++) {
        double *a = new double[n];
        double *b = new double[n];
        double *c = new double[n + offset];

        for (i = 0; i < n; i++) {
            b[i] = c[i] = i;
        }

        gettimeofday(&start_time, NULL); 
        #pragma omp parallel for schedule(static) collapse(2) private(i, repeat) default(shared)
        for (repeat = 0 ; repeat < num_repeats; repeat++) {
                // #pragma omp parallel for schedule(static) private(i)
                for (i = 0; i < (n - offset); i++) {
                    // a[i] = b[i] + c[i + offset];
                    a[i] = b[i] + c[i + offset] + i_dependent_fn(i); 
                    }
                }

        gettimeofday(&end_time, NULL); 

        double elapsed_time_static = (end_time.tv_sec - start_time.tv_sec) +            // Time for the nested loops, for increased accuracy
                                     (end_time.tv_usec - start_time.tv_usec) / 1e6;
        double time_per_iteration_static = elapsed_time_static / num_repeats;           // Normalised time per iteration
        
        cout<< "Static scheduling: for n = " << n << " and offset = " << offset <<", average wall time = " << time_per_iteration_static << " seconds" << endl;
        total_time_static += time_per_iteration_static;
        // double time_per_iteration_static_difference = time_per_iteration_static - last_time;
        // last_time = time_per_iteration_static;

        // n is the amount of doubles per array, 8 bytes per double, n->kB = n * 8 / 1024
        // outfile << n_kB << "," << time_per_iteration_static<< "," << time_per_iteration_static_difference << "\n";
        // outfile << offset << "," << time_per_iteration_static<< "," << time_per_iteration_static_difference << "\n";

        gettimeofday(&start_time, NULL);
        #pragma omp parallel for schedule(dynamic) collapse(2) private(i, repeat) default(shared)
        for (int repeat = 0 ; repeat < num_repeats; repeat++) {
                for (i = 0; i < (n - offset); i++) {
                    // a[i] = b[i] + c[i + offset];
                    a[i] = b[i] + c[i + offset] + i_dependent_fn(i); 
                    }
                }
        gettimeofday(&end_time, NULL); 

        double elapsed_time_dynamic = (end_time.tv_sec - start_time.tv_sec) +
                                      (end_time.tv_usec - start_time.tv_usec) / 1e6;
        double time_per_iteration_dynamic = elapsed_time_dynamic / num_repeats;
        cout<< "Dynamic scheduling: for n = " << n << " and offset = " << offset <<", average wall time = " << time_per_iteration_dynamic << " seconds" << endl;
        total_time_dynamic += time_per_iteration_dynamic;

        double n_kB = n * 8 / 1024.0;
        outfile << n_kB << "," << time_per_iteration_static << "," << time_per_iteration_dynamic << "\n";


        delete[] a;
        delete[] b;
        delete[] c;
    }
    outfile.close();
    cout<<"Total time with dynamic scheduling = " << total_time_dynamic << " seconds" << endl;
    cout<<"Total time with static scheduling = " << total_time_static << " seconds" << endl;

    return 0;
}



