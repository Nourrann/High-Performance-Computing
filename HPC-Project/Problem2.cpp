// Nada Alaa Eldeen         20200589
// Nouran Ahmed Abdelaziz   20200609
// Mariam Hany Gamal        20200532

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>

double sqrt(double n)
{
    double p = 0.0001;
    double ans = n / 2.0;
    while (fabs(ans * ans - n) > p)
    {
        ans = (ans + n / ans) / 2.0;
    }
    return ans;
}

int main()
{
    int size;
    double* arr, global_sum = 0.0, mean, variance = 0.0, std_deviation;

    double start = omp_get_wtime();
    printf("Enter the array size: \n");
    scanf("%d", &size);
    arr = (double*)malloc(size * sizeof(double));
    printf("Enter the array elements: \n");
    for (int i = 0; i < size; i++) {
        scanf("%lf", &arr[i]);
    }

#pragma omp parallel default(shared)
    {
        int number_threads, thread_id, partition_size, a, b;
        double local_sum = 0.0;

        number_threads = omp_get_num_threads();
        thread_id = omp_get_thread_num();
        partition_size = size / number_threads;
        a = thread_id * partition_size; // start of the partition
        b = a + partition_size; // b of the partition

        for (int i = a; i < b; i++)
            local_sum += arr[i];

        // condition to handle the remainder, if this is the last thread, we sum the remaining of the array
        // to the local_sum
        if (thread_id == number_threads - 1)
        {
            for (int i = b; i < size; i++)
                local_sum += arr[i];
        }

        // critical section to make one thread at a time to sum all local_sum in global_sum
#pragma omp critical
        {
            global_sum += local_sum;
        }
    }

    // Calculate mean 
    mean = (double)global_sum / size;
    double local_diff;
#pragma omp parallel default(shared)
    {
#pragma omp single
        {
#pragma omp parallel default(shared)
            {
                local_diff = 0.0;
#pragma omp for reduction(+:local_diff)
                for (int i = 0; i < size; i++) {
                    local_diff += ((arr[i] - mean) * (arr[i] - mean));
                }

#pragma omp critical
                {
                    variance += local_diff;
                }
            }
        }
    }

    variance /= size;
    std_deviation = sqrt(variance);
    double end = omp_get_wtime();
    double total_time = (end - start) / CLOCKS_PER_SEC;

    // output mean, variance, and standard deviation at the end of the program
    printf("Mean: %lf\n", mean);
    printf("Variance: %lf\n", variance);
    printf("Standard Deviation: %lf\n", std_deviation);
    printf("Time: %lf s\n", total_time);


    free(arr);
    return 0;
}