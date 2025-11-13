### ⚡ Parallel MPI Password Cracker

This project implements a parallel brute-force password search using MPI (Message Passing Interface).
It evolves from a serial C program that iterates through all possible passwords to find one that matches a given hash, into a distributed parallel system capable of scaling across dozens of processes.

The final version was executed on a high-performance cluster using 40 MPI processes to recover a 4-character password hash provided in the server.

🧩 1. From Serial to Parallel — The Journey

### Serial Baseline (find_serial_baseline.c)

The serial code (find_serial_baseline.c) sequentially tested all possible passwords from a defined character set until a match was found.
While correct, this approach is computationally expensive — as the password length and alphabet size grow, the total combinations increase exponentially (e.g., 64⁴ ≈ 16.7M combinations).

Limitations of the Serial Version

- Only utilizes a single CPU core

- No early termination across parallel instances

- Cannot scale with available cluster resources

The goal: parallelize this process using MPI to divide the search space efficiently and coordinate early stopping once the correct password is found .

### Designing the MPI Version (find_passwd_mpi.c)

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
| Stage             | Description                        | MPI Function    |
| ----------------- | ---------------------------------- | --------------- |
| Initialization    | Start MPI environment              | `MPI_Init`      |
| Work Distribution | Divide search space among ranks    | `rank`, `size`  |
| Local Search      | Each process hashes and compares   | local loop      |
| Early Exit        | Detect if any process found result | `MPI_Allreduce` |
| Broadcast         | Share winning password to all      | `MPI_Bcast`     |
| Timing            | Aggregate wall time                | `MPI_Reduce`    |
| Cleanup           | Graceful termination               | `MPI_Finalize`  |

### ⚙️ 3. Building and Running
Build
```bash
mpicc -O2 -Wall -o find_passwd_mpi.x src/find_passwd_mpi.c -lcrypt
```
Run (Example: 8 Processes)
```bash
mpirun -np 8 ./find_passwd_mpi.x
```
Slurm Job (40 Processes)
```bash
sbatch scripts/job_40.sh
```
scripts/job_40.sh runs the job on two nodes with 20 processes each and outputs:

Rank that found the password

Password string

Total space searched

Wall-clock execution time

### 🧪 4. Experimental Results (Cluster Runs)
| Password Type | MPI Processes | Password | Runtime (s) |
| ------------- | ------------- | -------- | ----------- |
| 3-character   | 40            | `WLU`    | 2.847       |
| 3-character   | 40            | `cp6`    | 0.810       |
| 3-character   | 40            | `123`    | 0.442       |
| 3-character   | 20            | `XYZ`    | 3.798       |
| 3-character   | 5             | `123`    | 1.598       |
| 3-character   | 2             | `XYZ`    | 26.401      |
| 4-character   | 40            | `L4uD`   | 87.058      |

<img width="884" height="228" alt="Ass1a" src="https://github.com/user-attachments/assets/ce1a72af-e4de-4413-a449-c45c98d3d7fd" />

<img width="787" height="187" alt="Ass1b" src="https://github.com/user-attachments/assets/061cba5f-59a5-4c3a-ab89-dc3a1f789dc9" />

<img width="892" height="511" alt="Ass1c" src="https://github.com/user-attachments/assets/10ad5ccc-588c-436f-b7c9-4367e9363404" />

<img width="913" height="831" alt="Ass1d" src="https://github.com/user-attachments/assets/c51ad8ba-b270-451d-b815-4278b5222358" />

<img width="882" height="412" alt="Ass1e" src="https://github.com/user-attachments/assets/78dbade5-27da-4ff2-b828-a422a1327e23" />




### 📊 5. Discussion and Analysis

- Performance Gains

The MPI version exhibited near-linear speedup for smaller search spaces:

Increasing from 2 → 5 → 20 → 40 ranks significantly reduced runtime.

Overhead from MPI communication was negligible compared to computation.

- Scalability Limitations

For longer passwords (4-char), even with 40 processes, total runtime (~87s) remained high because:

Search space grows exponentially (O(64ⁿ))

Hash computation dominates per iteration

Speedup saturates once computation outweighs distribution overhead

- Early Termination Efficiency

The MPI_Allreduce synchronization ensures all ranks terminate almost immediately after discovery, minimizing wasted cycles.
In tests, termination delay was <1 second across all nodes.
### 🔍 6. Key Takeaways

✅ Successfully transformed a serial brute-force program into a parallel, scalable MPI system.

✅ Demonstrated strong scaling for multi-node environments.

✅ Validated correctness and efficiency using both 3- and 4-character hashes.

✅ Highlighted the real-world analogy to distributed cryptanalysis and load-balanced search problems.
