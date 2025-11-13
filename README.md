### ⚡ Parallel MPI Password Cracker

This project implements a parallel brute-force password search using MPI (Message Passing Interface).
It evolves from a serial C program that iterates through all possible passwords to find one that matches a given hash, into a distributed parallel system capable of scaling across dozens of processes.

The final version was executed on a high-performance cluster using 40 MPI processes to recover a 4-character password hash provided in the server.

🧩 1. From Serial to Parallel — The Journey
🔹 Serial Baseline (find_serial_baseline.c)

The provided serial code (find_serial_baseline.c) sequentially tested all possible passwords from a defined character set until a match was found.
While correct, this approach is computationally expensive — as the password length and alphabet size grow, the total combinations increase exponentially (e.g., 64⁴ ≈ 16.7M combinations).

Limitations of the Serial Version

Only utilizes a single CPU core

No early termination across parallel instances

Cannot scale with available cluster resources

➡️ The goal: parallelize this process using MPI to divide the search space efficiently and coordinate early stopping once the correct password is found .

🔹 Designing the MPI Version (find_passwd_mpi.c)

To parallelize the search:

Initialize MPI Environment
Each process (rank) is assigned a unique ID and total process count using MPI_Comm_rank and MPI_Comm_size.

Divide the Search Space
Each process explores every n-th candidate in a round-robin fashion:

```bash
for (index = rank; index < total; index += size)
    test_password(index);
```
This ensures non-overlapping partitions and balanced load distribution.

Compute and Compare Hashes
Each process generates password candidates, encrypts them with crypt(), and compares against the target hash.

Early Termination (Synchronization)
When one process finds the password, it sets a local flag.
Using MPI_Allreduce with logical OR, all processes detect this and exit cleanly:
```bash
MPI_Allreduce(&found_local, &found_any, 1, MPI_INT, MPI_LOR, MPI_COMM_WORLD);
```
Reporting and Timing

MPI_Wtime() measures local execution time.

MPI_Reduce() computes the maximum wall time across all ranks.

The rank that found the password reports it to all others via MPI_Bcast.

Scaling to Four-Character Passwords
After verifying functionality for 3-character passwords, the program was modified to handle 4-character combinations by adjusting:
```bash
total = pow(ALPHABET_SIZE, PASSWORD_LENGTH);
```
and running with export PASS_LEN=4.

### 🧠 2. Architecture Overview
