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

#### NOTE: mpiCC -> C++ | mpicc -> C

- The code is based on Open MPI which may be pre-installed on your linux distro.
- Type the following command to see location of the library.
```
mpiCC --showme
```
- If not installed, type the following command to install binaries, documentation and lib files of Open MPI.
```
sudo apt install openmpi-bin openmpi-doc libopenmpi-dev
```
- Open MPI is generally stored under
```
/usr/include/x86-64-linux-gnu/
```
- Make sure the path is included in your includePath as well as path to your binaries.
- Use the following command to compile the MPI code.
```
mpiCC filename.cpp -o filename.exe
```
- Use mpirun or mpiexec command to run the code. (here 10 is the number of MPI Process that will execute)
```
mpirun --hostfile hostfile.txt -np 4 filename.exe
```

### Configuration
- Change the expressions (marked with *** comments) according to the integral you are calculating (you may contact me for a sequential implementation of the code that uses of muParser to parse expressions)
- Please change the config file and replace the IP address with that of your devices in the cluster
- Set slots as per your need | slots are amount of process that will run on a device
- Use --oversubscribe to run processes > number of cores

### Issues
- Code gets stuck at MPI_FINALIZE() which I found is a common and recurring issue after some research. Ctrl + C to force exit.

## Additional Material
- Use this guide to create cluster with your devices https://github.com/adeen-atif/MPI-Cluster#readme
- Use this guide to run WSL on your Windows machine to run Unix-based code with VSCode https://github.com/MuhammadHabibKhan/array-sum-pthreads/blob/main/Guide.md 
