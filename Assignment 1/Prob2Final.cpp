
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mpi.h"

//Nouran Ahmed 20200609
//Nada Alaa 20200589

int i, j, k;

MPI_Status status;

// Whether to print the matrix when completed
bool printResults = true;

//The number of rows and cols for each of the two matrices 
int rows1, cols1, rows2, cols2;
int** matrix1, ** matrix2;

int main(int argc, char** argv)
{


    printf("Enter the dimensions of matrix 1: \n");
    scanf("%d", &rows1);
    scanf("%d", &cols1);
    printf("Enter the dimensions of matrix 2: \n");
    scanf("%d", &rows2);
    scanf("%d", &cols2);


    // to allocate the matrices
    matrix1 = (int**)malloc(rows1 * sizeof(int*));
    for (i = 0; i < rows1; i++)
    {
        matrix1[i] = (int*)malloc(cols1 * sizeof(int));
    }

    matrix2 = (int**)malloc(rows2 * sizeof(int*));
    for (i = 0; i < rows1; i++)
    {
        matrix2[i] = (int*)malloc(cols2 * sizeof(int));
    }



    int productMatrix[rows1][cols2];


    int numberOfProcessors;
    int processorRank;
    int numberOfWorkers;
    int tmp;

    // Processor sending data
    int sourceProcessor;

    // Processor to receive data
    int destinationProcessor;

    // the number of rows and cols that each slave will work on
    int r1, r2, c1, c2;



    MPI_Init(&argc, &argv);

    // define the number of the processors
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcessors);

    // define the rank of each processor
    MPI_Comm_rank(MPI_COMM_WORLD, &processorRank);

    // to determine number of slaves
    numberOfWorkers = numberOfProcessors - 1;

    // if it's the master
    if (processorRank == 0)
    {

        printf("Number of slaves: %d \n", numberOfWorkers);

        printf("Enter the elements of matrix 1: \n");
        // Populate the matrices with values
        for (i = 0; i < rows1; i++)
        {
            for (j = 0; j < cols1; j++)
            {
                scanf("%d", &(matrix1[i][j]));
            }
        }
        printf("Enter the elements of matrix 2: \n");
        // Populate the matrices with values
        for (i = 0; i < rows2; i++)
        {
            for (j = 0; j < cols2; j++)
            {
                scanf("%d", &(matrix2[i][j]));
            }
        }



        // to determine the work for each slave 
        for (destinationProcessor = 1; destinationProcessor <= numberOfWorkers; destinationProcessor++)
        {
            MPI_Send(&rows1, 1, MPI_INT, destinationProcessor, 1, MPI_COMM_WORLD);
            MPI_Send(&cols1, 1, MPI_INT, destinationProcessor, 2, MPI_COMM_WORLD);
            MPI_Send(&rows2, 1, MPI_INT, destinationProcessor, 3, MPI_COMM_WORLD);
            MPI_Send(&cols2, 1, MPI_INT, destinationProcessor, 4, MPI_COMM_WORLD);
        }

        // for loop to recieve the work of each slave
        for (i = 1; i <= numberOfWorkers; i++)
        {
            for (j = 0; j < rows1; j++)
            {
                MPI_Send(matrix1[j], cols1, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
            for (j = 0; j < rows2; j++)
            {
                MPI_Send(matrix2[j], cols2, MPI_INT, i, 0, MPI_COMM_WORLD);
            }

        }



        int src;
        for (src = 1; src <= numberOfWorkers; src++)
        {
            for (i = 0; i < rows1; i++)
            {
                for (j = 0; j < cols2; j++)
                {
                    MPI_Recv(&tmp, 1, MPI_INT, src, 0, MPI_COMM_WORLD, &status);
                    productMatrix[i][j] = tmp;
                }

            }
        }


        // to print each of the 3 matrices
        if (printResults == true)
        {
            printf("Matrix 1:\n");
            for (i = 0; i < rows1; i++)
            {
                for (j = 0; j < cols1; j++)
                {
                    printf("%d\t", matrix1[i][j]);
                }
                printf("\n");
            }
            printf("Matrix 2:\n");

            for (i = 0; i < rows2; i++)
            {
                for (j = 0; j < cols2; j++)
                {
                    printf("%d\t", matrix2[i][j]);
                }
                printf("\n");
            }
            printf("Product Matrix:\n");
            for (i = 0; i < rows1; i++)
            {
                for (j = 0; j < cols2; j++)
                {
                    printf("%d\t", productMatrix[i][j]);
                }
                printf("\n");
            }


        }
        free(matrix1);
        free(matrix2);
    }


    // if it's slave
    if (processorRank > 0)
    {
        sourceProcessor = 0;
        MPI_Recv(&rows1, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&cols1, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows2, 1, MPI_INT, 0, 3, MPI_COMM_WORLD, &status);
        MPI_Recv(&cols2, 1, MPI_INT, 0, 4, MPI_COMM_WORLD, &status);

        matrix1 = (int**)malloc(rows1 * sizeof(int*));
        for (i = 0; i < rows1; i++)
        {
            matrix1[i] = (int*)malloc(cols1 * sizeof(int));
        }

        matrix2 = (int**)malloc(rows2 * sizeof(int*));
        for (i = 0; i < rows2; i++)
        {
            matrix2[i] = (int*)malloc(cols2 * sizeof(int));
        }

        for (j = 0; j < rows1; j++)
        {
            MPI_Recv(matrix1[j], cols1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        for (j = 0; j < rows2; j++)
        {
            MPI_Recv(matrix2[j], cols2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        int productMatrix[rows1][cols2];

        //for loop to multiply two matrices
        for (k = 0; k < rows1; k++)
        {
            for (i = 0; i < cols2; i++)
            {
                productMatrix[k][i] = 0;
                for (j = 0; j < cols1; j++)
                {
                    productMatrix[k][i] = productMatrix[k][i] + matrix1[k][j] * matrix2[j][i];
                }
            }
        }

        for (i = 0; i < rows1; ++i)
        {
            for (j = 0; j < cols2; ++j)
            {
                tmp = productMatrix[i][j];
                MPI_Send(&tmp, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            }
        }
        free(matrix1);
        free(matrix2);

    }


    MPI_Finalize();
}
