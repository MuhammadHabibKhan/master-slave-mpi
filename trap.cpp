/*
IMPLEMENTATION OF THE NUMERICAL METHOD: TRAPEZOIDAL RULE OF INTEGRATION TO ESTIMATE THE DEFINITE INTEGRAL USING MPI CLUSTER

> (***) Change these expressions according to the integral you are calculating | see my sequential implementation to see the use of muParser to parse expressions
> TAG = 1 for result communication | TAG = 0 for initial data communcation
> Please change the config file and replace the IP address with that of your devices in the cluster
> Set slots as per your need | slots are max amount of process that may run on a device | use --oversubscribe to run processes > number of cores
> Built using openMPI | Pre-installed in most Linux distros today. Use mpiCC / mpicc --showme to see location of the library
> If not present, use "sudo apt install openmpi-bin openmpi-doc libopenmpi-dev" to install binaries, documentation and library of openMPI
> Generally present under /usr/include/x86-64-linux-gnu/
> NOTE: Code gets stuck at MPI_FINALIZE() which is a common and recurring issue. Ctrl + C to force exit

Muhammad Habib Khan | B20102088
*/

#include <mpi.h>
#include <iostream>
#include <vector>

using namespace std;

class Trapezoidal
{
public:

    // defining a seperate struct to send and receive data required
    typedef struct
    {
        long double min; // lower limit of the definite integral
        long double width; // width of each slice / division
        
        int workLoadTotal; // divisions for each MPI process
        int startIndex; // use of index to xply with correct division in trapezoidal rule
        int extraWorkLoad; // Extra Work calculated in round-robin fashion

    } calcData;

    long double max;
    long double min;
    long double width;
    long int divisions;

    Trapezoidal(long double max, long double min, long int n)
    {
        this->max = max;
        this->min = min;
        this->divisions = n;

        width = (max - min) / divisions;
    }

    void master()
    {
        int totalProcess;
        MPI_Comm_size(MPI_COMM_WORLD, &totalProcess);

        totalProcess -= 1; // since no work assigned to rank 0 / master

        // calculate workload for each MPI Process
        int workLoad = divisions / totalProcess; // implict cast to int
        int extraWorkLoad = 0;
        
        // assigning extra work load to MPI Processes
        if (divisions % totalProcess != 0)
        {
            extraWorkLoad = divisions % totalProcess;
        }
        // place extra work load for process into a vector
        int index = 0;
        vector <int> ExtraWorkLoadVector(totalProcess);

        // round-robin fashion assignment of EWL
        while (extraWorkLoad != 0)
        {
            ExtraWorkLoadVector[index % totalProcess] += 1;
            extraWorkLoad -= 1;
            index += 1;
        }

        // using calcData struct to send slaves the data required to calculate the looped term of trapezoidal rule
        for (int i = 0; i < totalProcess; i++)
        {
            calcData Data;
            
            Data.min = min;
            Data.width = width;
            Data.workLoadTotal = workLoad + ExtraWorkLoadVector[i]; // combine the workload and extra workload (if any) for the process
            Data.startIndex = workLoad * i; // xply the loop counter by workload / division amount to jump over

            int result;
            result = MPI_Send(&Data, sizeof(calcData), MPI_BYTE, i + 1, 0, MPI_COMM_WORLD); // using i + 1 as destination rank to target ranks starting from 1 to N
            // as rank 0 (master) does not need to receive data & we set totalProcess variable to the actual usable amount previously

            // error handling to output the error encountered
            if (result != MPI_SUCCESS) 
            {
                char error_string[MPI_MAX_ERROR_STRING];
                MPI_Error_string(result, error_string, NULL);

                cout << "Error Occurred: " << error_string << endl;
            }
            cout << "-------------------------------------------------------------------" << endl;
            cout << "Master Sent Data to Slave " << i + 1 << endl;
        }

        long double integral = 0;

        // receive the looped term calculated by slaves over the defined number of divisions
        // add the end terms and xply the final result by width to yield the integral

        for (int i = 0; i < totalProcess; i++)
        {
            long double tempIntegral = 0; // temp variable to hold calculation for looped term of each MPI Process
            MPI_Recv(&tempIntegral, 1, MPI_LONG_DOUBLE, i + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // using i + 1 for same reasons as above

            integral += tempIntegral; // add term to main integral

            cout << "-------------------------------------------------------------------" << endl;
            cout << "Master received data from Slave " << i + 1 << " : " << tempIntegral << endl; 
        }
        // do the final calculations for trapezoidal rule on main process
        long double startTerm = min * min; // (***)
        long double endTerm = max * max; // (***)

        integral += (startTerm + endTerm) / 2; // Add the end terms
        integral *= width; // xply with width

        cout << "-------------------------------------------------------------------" << endl;
        cout << "Final Integral: " << integral << endl;
        cout << "-------------------------------------------------------------------" << endl;
    }

    void slave()
    {
        // receive data, perform calculations and send results back

        int totalProcess; // using local variable to avoid memory issues in MPI
        long double loopedTerm = 0;
        
        MPI_Comm_size(MPI_COMM_WORLD, &totalProcess);
        totalProcess -= 1;

        for (int i = 0; i < totalProcess; i++)
        {
            calcData dataReceived;

            MPI_Recv(&dataReceived, sizeof(calcData), MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // receive data in struct form

            // store the data from struct in local variables
            long double min = dataReceived.min;
            long double width = dataReceived.width;
            int divisions = dataReceived.workLoadTotal;
            int startIndex = dataReceived.startIndex;

            int rank;
            MPI_Comm_rank(MPI_COMM_WORLD, &rank);

            cout << "-------------------------------------------------------------------" << endl;
            cout << "Slave " << rank << " data received" << endl;
            cout << "| min: " << min << " | width: " << width << " | div: " << divisions << " | index: " << startIndex << endl;

            for (int i = 0; i < divisions; i++)
            {
                long double x = min + (width * startIndex);
                loopedTerm += (x * x); // integral x^2 (***)
                startIndex++;    
            }

            MPI_Send(&loopedTerm, 1, MPI_LONG_DOUBLE, 0, 1, MPI_COMM_WORLD); // send the calculated looped term back to master to add into the integral

            cout << "-------------------------------------------------------------------" << endl;
            cout << "Slave " << rank << " data sent to master: " << loopedTerm << endl;
        }
    }

    void mpi(int argc, char** argv)
    {
        int rank;

        MPI_Init(&argc, &argv);
        MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        
        if (rank == 0) master();
        else slave();

        MPI_Finalize();
    }
};

int main(int argc, char** argv)
{
    Trapezoidal Rule = Trapezoidal(10, 0, 1000000);
    Rule.mpi(argc, argv);
}