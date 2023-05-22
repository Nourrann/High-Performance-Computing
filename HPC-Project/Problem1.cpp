// Nouran Ahmed Abdelaziz 20200609
// Mariam Hany Gamal 20200532
// Nada Alaa Eldin 20200589

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


// function to calculate the square root of a number
double squareRoot(double n)
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
    int rank, size, r, remainder, i, total_sum = 0, sum = 0, partialSum = 0, totalPartialSum = 0, arrSize;
    double mean, variance, sd; //sd: standard deviation

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    clock_t begin = clock();

    if (rank == 0)
    {
        printf("Enter array size: \n");
        scanf("%d", &arrSize);
        printf("Enter array elements: \n");
        r = arrSize / (size - 1); // partial size for each slave
        remainder = arrSize % (size - 1);
    }

    MPI_Bcast(&arrSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&r, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&remainder, 1, MPI_INT, 0, MPI_COMM_WORLD);


    int arr[arrSize];
    if (rank == 0)
    {
        for (i = 0; i < arrSize; i++)
        {
            scanf("%d", &arr[i]);
        }
    }
    MPI_Bcast(arr, arrSize, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank != 0)
    {
        int a = (rank - 1) * r; //start index for each slave
        int b = a + r - 1;      //end index for each slave

        if (rank == size - 1)
            b += remainder;

        for (i = a; i <= b; i++)
        {
            sum += arr[i];
        }
    }
    // reduce the sum of all slaves in one variable (total_sum)
    MPI_Reduce(&sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        mean = (double)total_sum / arrSize;
        printf("Mean = %.2f\n", mean);

    }
    MPI_Bcast(&mean, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank != 0)
    {
        int a = (rank - 1) * r;
        int b = a + r - 1;

        if (rank == size - 1)
            b += remainder;

        for (i = a; i <= b; i++)
        {
            partialSum += ((arr[i] - mean) * (arr[i] - mean));
        }
    }

    MPI_Reduce(&partialSum, &totalPartialSum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        variance = (double)totalPartialSum / arrSize;
        printf("Variance = %lf \n", variance);
        sd = squareRoot(variance);
        printf("Standard Deviation = %lf \n", sd);
        clock_t end = clock();
        double runTime = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("Runtime: %f seconds\n", runTime);
    }

    MPI_Finalize();
    return 0;
}