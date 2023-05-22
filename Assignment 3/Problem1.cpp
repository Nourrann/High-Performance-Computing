// Nada Alaa ELdeen Ali     20200589
// Nouran Ahmed Abdelaziz   20200609
#include <stdio.h>
#include <string.h>
#include "mpi.h"

#define MAX_SIZE 1000

int main(int argc, char *argv[])
{
    int size , rank, key, length, i;
    char message[MAX_SIZE], Encrypt_message[MAX_SIZE];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // master is working
    if (rank == 0)
    {
        FILE *fp;
        fp = fopen("plaintext", "r");

        // reads the message from file
        fgets(message, MAX_SIZE, fp);
        fclose(fp);


        printf("Enter the key: \n");
        scanf("%d", &key);

        MPI_Bcast(&key, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatter(message, MAX_SIZE / size, MPI_CHAR, message, MAX_SIZE / size, MPI_CHAR, 0, MPI_COMM_WORLD);

        // handling the remainder
        int remaining_size = length % (MAX_SIZE / size);
        if (remaining_size > 0)
        {
            int start_index = (length / (MAX_SIZE / size)) * (MAX_SIZE / size);
            int end_index = start_index + remaining_size;
            for (int i = end_index - 1; i >= start_index; i--) {
                message[i + (MAX_SIZE / size) - remaining_size] = message[i];
            }
        }
    }
    else{
        MPI_Bcast(&key, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatter(message, MAX_SIZE / size, MPI_CHAR, message, MAX_SIZE / size, MPI_CHAR, 0, MPI_COMM_WORLD);
    }

    length = strlen(message);
    // encrepting the message
    for (i = 0; i < length; i++)
    {
        if (message[i] >= 'A' && message[i] <= 'Z')
            Encrypt_message[i] = 'A' + (message[i] - 'A' + key) % 26;

        else if (message[i] >= 'a' && message[i] <= 'z')
            Encrypt_message[i] = 'a' + (message[i] - 'a' + key) % 26;

        else Encrypt_message[i] = message[i];
    }

    MPI_Gather(Encrypt_message, MAX_SIZE / size, MPI_CHAR, Encrypt_message, MAX_SIZE / size, MPI_CHAR, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {

        FILE *fp;
        fp = fopen("ciphertext.txt", "w");

        printf("Encrypted message: %s", Encrypt_message);
        // put the encrypted message in the outupt file
        fputs(Encrypt_message, fp);
        fclose(fp);
    }

    MPI_Finalize();
}
