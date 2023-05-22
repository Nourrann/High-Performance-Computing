// 20200589 Nada ALaa Eldeen Ali
// 20200609 Nouran Ahmed Abdelaziz

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int main(int argc, char** argv)
{
    int rank, size, remainder, r, y, x, a, b, sub_count = 0, total_count = 0, i;
    double Total_RunTime;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    clock_t begin = clock();
    if (rank == 0)
    {

        printf("Enter lower bound: \n");
        scanf("%d", &x);
        printf("Enter upper bound: \n");
        scanf("%d", &y);

        //subrange size
        r = ((y - x) + 1) / (size - 1);
        // remainder for last slave
        remainder = ((y - x) + 1) % (size - 1);

        for (i = 1; i < size; i++)
        {
            MPI_Send(&x, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&r, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&remainder, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        for (i = 1; i < size; i++)
        {
            MPI_Recv(&sub_count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_count += sub_count;
        }

        printf("Total total_count of primes between %d and %d is %d\n", x, y, total_count);
        clock_t end = clock();
        Total_RunTime = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("Runtime: %f seconds\n", Total_RunTime);

    }
    else {
        MPI_Recv(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&r, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&remainder, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        a = x + (rank - 1) * r;
        b = a + r - 1;
        if (rank == size - 1)
            b += remainder;

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
        MPI_Send(&sub_count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        printf("p%d: calculate partial total_count of prime numbers from %d to %d -> Count = %d \n", rank, a, b, sub_count);
    }
    MPI_Finalize();
    return 0;
}