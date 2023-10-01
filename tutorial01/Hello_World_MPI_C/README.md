### Usage

Compile the code by `make clean; make`. A binary named `hello` will be created. To run the test, simply do
```bash
mpirun -np <nproc> ./hello
```
or on Slurm clusters
```bash
srun -n <nproc> ./hello
```

### Description of the test
This is a simple hello world MPI program written in C.

