#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
    // Hash to be matched. Each student has a unique one in password_hash.txt
    const char *const hash = "$1$mL.JafQu$eNgS848N4gbdT4T8LrC5z1"; 

    // Number of characters in the character set
    int num_pass_chars = 64;
    // Length of the password we are testing
    int pass_len = 4;
    // Array to store generated passwords
    char passwd[5];
    // The character set used in password generation
    const char *const passchars =
        "./0123456789ABCDEFGHIJKLMNOPQRST"
        "UVWXYZabcdefghijklmnopqrstuvwxyz";

    // Compute total number of possible passwords
    long long int total_passwords = 1;
    int i;
    for (i = 0; i < pass_len; i++)
        total_passwords *= num_pass_chars;

    int rank, size, found = 0;
    double start_time, end_time, local_time, max_time;

    MPI_Init(&argc, &argv); // Initialize MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get rank of process
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Get total number of processes

    start_time = MPI_Wtime(); // Record start time

    // Each process checks every Nth password, where N is the number of processes
    for (i = rank; i < total_passwords && !found; i += size) {
        long long int itest = i;
        int j;
        for (j = 0; j < pass_len; j++) {
            passwd[j] = passchars[itest % num_pass_chars];
            itest /= num_pass_chars;
        }
        passwd[pass_len] = '\0'; // Null-terminate the string

        // Hash the generated password and compare
        char *result = crypt(passwd, hash);
        if (strcmp(result, hash) == 0) {
            printf("Process %d found password: %s\n", rank, passwd);
            found = 1;
        }

        // Let all processes know if one has found the password
        MPI_Allreduce(MPI_IN_PLACE, &found, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
        if (found) break;
    }

    end_time = MPI_Wtime(); // Record end time
    local_time = end_time - start_time;

    // Gather the maximum execution time across all processes for reporting
    MPI_Reduce(&local_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Total password space: %lld\n", total_passwords);
        printf("Number of MPI processes: %d\n", size);
        printf("Execution time (max across all processes): %f seconds\n", max_time);
    }

    MPI_Finalize(); // Finalize MPI
    return 0;
}
