# master-slave-mpi
Master-Slave / Worker-Supervisor Architecture Implemented using Open MPI in C++

## About
- This code implements the numerical integration method 'Trapezoidal Rule' to calculate the definite integral over the provided lower and upper limits using Open MPI.

### Trapezoidal Rule
- It estimates the integral using the following formula.

![trapezoidal](https://github.com/MuhammadHabibKhan/master-slave-mpi/assets/92048010/58579405-6d6d-45d6-9f6c-9c287f5ba186) &nbsp;

The greater the number of divisions, the more accurate the estimated calculation of the integral becomes (see the gif below | source: wikipedia)

![Trapezium2](https://github.com/MuhammadHabibKhan/master-slave-mpi/assets/92048010/a785ac3d-f084-4c5b-a2f8-6696bc3a94f7)

#### Calculations
- The looped part of the formula is calculated as per how many divisions you want in your calculation. This workload is divided among the different MPI processes for faster calculation. 
- However, in this current implementation, I have not assigned any workload to master or rank 0. It only calculates the end terms and multiplies the result with width.

### Running the Code
- Built using Open MPI | Pre-installed in most Linux distros today. Use mpiCC / mpicc --showme to see location of the library.
- If not present, use "sudo apt install openmpi-bin openmpi-doc libopenmpi-dev" to install binaries, documentation and lib files of Open MPI.
- Generally present under /usr/include/x86-64-linux-gnu/ | Make sure the path is included in your includePath as well as path to your binaries/
- Use mpicc <filename>.c -o <filename>.exe | mpiCC <filename>.cpp -o <filename>.exe to compile the MPI code.
- Use mpirun or mpiexec to run the code | mpirun --hostfile <HOST_FILE.txt> -np <NO_MPI_PROCESS> <File_name>.exe  (replace content inside <> accordingly) 

### Configuration
- Change the expressions (marked with *** comments) according to the integral you are calculating (see my sequential implementation to see the use of muParser to parse expressions)
- Please change the config file and replace the IP address with that of your devices in the cluster
- Set slots as per your need | slots are amount of process that will run on a device
- Use --oversubscribe to run processes > number of cores

### Issues
- Code gets stuck at MPI_FINALIZE() which I found is a common and recurring issue after some research. Ctrl + C to force exit.

## Additional Material
- Use this guide to create cluster with your devices https://github.com/adeen-atif/MPI-Cluster#readme
