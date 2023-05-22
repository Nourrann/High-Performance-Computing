// 20200589 Nada ALaa Eldeen Ali
// 20200609 Nouran Ahmed Abdelaziz

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int main(int argc, char** argv)
{
    static long num_steps = 1000000;
    int rank, size, r, sub_count, remainder , i;
    double total_sum=0.0 , step , x, pi, sum = 0.0;

    step = 1.0/(double)num_steps;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //master is working
    if (rank == 0)
    {
        r = (num_steps+1) / (size - 1); //the partition size of each slave
        remainder = (num_steps+1) % (size - 1); //calculate the remainder
    }

    MPI_Bcast(&r, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&remainder, 1, MPI_INT, 0, MPI_COMM_WORLD);

    //slaves are working
    if(rank != 0)
    {
        //calculate the boundaries of each slave
        int a = (rank-1)*r;
        int b = a + r - 1;

        if (rank == size-1)
            b += remainder;

        for ( i = a ; i <= b ; i++)
        {
            x = ((double)(i + 0.5)*step);
            sum += 4.0/(1.0+x*x);
        }
    }
    MPI_Reduce(&sum, &total_sum, 1,  MPI_DOUBLE_PRECISION, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        //printf("total sum %lf: \n", total_sum);
        pi = step*total_sum;
        printf("%.20f \n", pi);
    }

    MPI_Finalize();
    return 0;
}
