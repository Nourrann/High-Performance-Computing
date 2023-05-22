// 20200589 Nada ALaa Eldeen Ali
// 20200609 Nouran Ahmed Abdelaziz

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <time.h>

int main(int argc, char** argv)
{
    int rank, size, x, y, r, sub_count, total_count = 0, remainder, i;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    clock_t begin = clock();
    if (rank == 0)
    {
        printf("Enter lower bound number x: \n");
        scanf("%d", &x);
        printf("Enter upper bound number y: \n");
        scanf("%d", &y);

        //subrange size
        r = ((y - x) + 1) / (size - 1);
        // remainder for last slave
        remainder = ((y - x) + 1) % (size - 1);
    }

    MPI_Bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&r, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&remainder, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank != 0)
    {
        int a = x + ((rank - 1) * r);
        int b = a + r - 1;

        if (rank == size - 1)
            b += remainder;
        sub_count = 0;


        for (i = a; i <= b; i++)
        {
            int j, c = 0;
            if (i <= 1) c++;
            for (j = 2; j * j <= i; j++)
            {
                if (i % j == 0)
                {
                    c++;
                    break;
                }
            }
            if (c == 0)
                sub_count++;

        }

        printf("p%d: calculate partial count of prime numbers from %d to %d -> Count = %d \n", rank, a, b, sub_count);

    }
    MPI_Reduce(&sub_count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    clock_t end = clock();
    if (rank == 0)
    {
        double Total_RunTime = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("Total count of prime numbers between %d and %d: %d\n", x, y, total_count);
        printf("Runtime: %f seconds\n", Total_RunTime);
    }

    MPI_Finalize();
    return 0;
}