#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


int main(int argc, char* argv[])
{

    int i, j;
    int rank, size;
    int arr[100000];
    int partition[100000]; // array for the partition of each slave
    int arrSize, partSize;
    int maxN, maxI; //maxN is the max number that we will return, maxI is the index of the max number 


    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        printf("Hello from master process.\n");
        printf("Number of slave processes is %d\n", size - 1);

        printf("Please enter size of array...\n");
        scanf("%d", &arrSize);
        printf("Please enter array elements...\n");
        for (i = 0; i < arrSize; i++)
        {
            scanf("%d", &arr[i]);
        }

        // to divide the work of each slave
        partSize = arrSize / (size - 1); //to calculate each partition size
        int remainder = arrSize % (size - 1); //to handle if there will be remainder when we divede the partition size for each slave

        int offset; //partition size when we have an odd slaves.
        for (i = 1; i < size; i++)
        {
            if (i == size - 1)
            {
                offset = partSize + remainder;// the last slave will take the remainder of the array
            }
            else
            {
                offset = partSize;
            }

            MPI_Send(&offset, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&arr[(i - 1) * partSize], offset, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        // we will recieve the maximum number and index from each slave 
        maxN = -1000000;
        for (i = 1; i < size; i++)
        {
            MPI_Recv(&maxN, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&maxI, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Hello from slave#%d Max number in my partition is %d and index is %d \n", i, maxN, maxI);
        }

        // for loop to find the maximum number and its index in the array for the master
        maxN = -10000000;
        maxI = -1;
        for (i = 0; i < arrSize; i++)
        {
            if (arr[i] > maxN)
            {
                maxN = arr[i];
                maxI = i;
            }
        }


        printf("Master process announce the final max which is %d and its index is %d.\n", maxN, maxI);
    }
    else if (rank != 0)
    {
        // recieve the partition of the array for each slave
        MPI_Recv(&partSize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&partition, partSize, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        maxN = -10000000;
        maxI = -1;
        for (i = 0; i < partSize; i++)
        {
            if (partition[i] > maxN)
            {
                maxN = partition[i];
                maxI = i;
            }
        }

        // send the maximum number and its index to master
        MPI_Send(&maxN, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&maxI, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

    }

    MPI_Finalize();
    return 0;
}
