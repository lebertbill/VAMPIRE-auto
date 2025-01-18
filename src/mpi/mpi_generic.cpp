#include <mpi.h> // Include the MPI header
#include <iostream>
#include <fstream>
#include <string>
#include "vmpi.hpp"

namespace vmpi {

MPI_Comm custom_comm; // Define the custom communicator

// Function to initialize MPI and set up the default communicator
void initialise(int argc, char *argv[]) {
    #ifdef MPICF
    int resultlen;
    char name[512];

    // Check if MPI is already initialized
    int initialized;
    MPI_Initialized(&initialized);
    if (!initialized) {
        // Initialize MPI
        MPI_Init(&argc, &argv);
    }

    // Get number of processors and rank in MPI_COMM_WORLD
    MPI_Comm_rank(MPI_COMM_WORLD, &vmpi::my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &vmpi::num_processors);

    // Get the processor name
    MPI_Get_processor_name(name, &resultlen);
    vmpi::hostname = name;

    // Start MPI Timer
    vmpi::start_time = MPI_Wtime();

    // Set master flag on master (root) process
    if (vmpi::my_rank == 0) vmpi::master = true;

    #else
    if (vmpi::my_rank == 0) vmpi::master = true;
    #endif
    std::cout << "Initialize MPI..." << std::endl;
    return;
}

// Function to split the communicator into sub-groups
void split_comm() {
    #ifdef MPICF
    // Synchronize before splitting the communicator
    MPI_Barrier(MPI_COMM_WORLD);

    // Split the communicator: custom logic can be added here
    int err = MPI_Comm_split(MPI_COMM_WORLD, vmpi::my_rank % 2, vmpi::my_rank, &vmpi::custom_comm);
    if (err != MPI_SUCCESS) {
        char err_string[MPI_MAX_ERROR_STRING];
        int len;
        MPI_Error_string(err, err_string, &len);
        std::cerr << "MPI_Comm_split failed: " << err_string << std::endl;
        MPI_Abort(MPI_COMM_WORLD, err);
    }

    // Synchronize after splitting the communicator
    MPI_Barrier(vmpi::custom_comm);

    // Get rank and size in the new communicator
    int custom_rank, custom_size;
    MPI_Comm_rank(vmpi::custom_comm, &custom_rank);
    MPI_Comm_size(vmpi::custom_comm, &custom_size);

    std::cout << "Process " << vmpi::my_rank << " in MPI_COMM_WORLD is now process "
              << custom_rank << " in custom_comm, with " << custom_size << " total processes.\n";

    // Set master flag on master (root) process in the new communicator
    if (custom_rank == 0) vmpi::master = true;
    else vmpi::master = false;  // Reset master flag for non-root processes

    #endif
}

// Function to free the custom communicator
void free_comm() {
    #ifdef MPICF
    // Synchronize before freeing the communicator
    MPI_Barrier(vmpi::custom_comm);

    // Free the custom communicator
    if (vmpi::custom_comm != MPI_COMM_NULL) {
        MPI_Comm_free(&vmpi::custom_comm);
    }

    // Synchronize after freeing the communicator
    MPI_Barrier(MPI_COMM_WORLD);
    #endif
}

// Function to finalize MPI and print the simulation time
int finalise() {
    #ifdef MPICF

    // Wait for all processors before finalizing
    MPI_Barrier(MPI_COMM_WORLD);

    // Stop MPI Timer
    vmpi::end_time = MPI_Wtime();

    // Output MPI simulation time
    if (vmpi::my_rank == 0) {
        std::cout << "MPI Simulation Time: " << vmpi::end_time - vmpi::start_time << std::endl;
    }

    // Check if MPI is already finalized
    int finalized;
    MPI_Finalized(&finalized);
    if (!finalized) {
        MPI_Finalize();
    }

    #endif

    return EXIT_SUCCESS;
}

// Function to display information about hosts
int hosts() {
    // Ensure MPI environment is initialized
    if (num_processors <= 512) {
        // Output rank, num_procs, hostname to screen
        std::cout << "Processor " << vmpi::my_rank + 1 << " of " << vmpi::num_processors
                  << " online on host " << vmpi::hostname << std::endl;
    }

    return EXIT_SUCCESS;
}

// Function to handle MPI timers
double SwapTimer(double OldTimer, double& NewTimer) {
    // Get current time
    double time = MPI_Wtime();

    // Set start time for NewTimer
    NewTimer = time;

    // Calculate time elapsed since last call for OldTimer
    return time - OldTimer;
}

} // end of namespace vmpi
