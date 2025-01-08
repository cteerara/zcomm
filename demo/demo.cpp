#include <iostream>
#include "zcomm_wrapper.h"


class DataClass
{
public:
    double a;
    DataClass(double _a=0) : a(_a){};
};
// ----------------------------------------------------------------------


int main(int argc, char** argv)
{

    // Initialize MPI variables
    MPI_Init(&argc, &argv);
    MPI_Comm comm = MPI_COMM_WORLD;
    int mpi_rank, mpi_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    // This demo is written specifically for 3 procs,
    // so make sure only 3 procs are being used
    if (mpi_size != 3)
    {
        printf("This demo is written for 3 mpi procs. Please run with 3 processes.\n");
        MPI_Finalize();
        return 0;
    }


    // Initialize export start ids. This is a nested list of where export_start_ids.size() = mpi_size.
    // Each entry contain the start index of the array to be sent to a specific process.
    // export_start_ids[r] = {i} with stride = s means that a contiguous block of size `s` starting at index `i` is to be
    // sent to process rank `r`.
    // For example if rank 0 has export_start_ids[2] = {0, 7}, and stride = 3, then
    // rank 0 will send entries with index 0, 1, 2,  7, 8, 9 to rank 2.

    // Build communicator for our 3 process demo
    int stride = 2;
    std::vector<std::vector<int>> export_start_ids(mpi_size);
    if (mpi_rank == 0)
        export_start_ids = { {}, {2, 5}, {7} };
    else if (mpi_rank == 1)
        export_start_ids = { {7}, {}, {3} };
    else if (mpi_rank == 2)
        export_start_ids = { {1, 4, 7}, {6}, {} };
    zcomm::ZComm<DataClass> zcomm = zcomm::ZComm<DataClass>(
            comm, export_start_ids, stride);

    // Build data array to be communicated
    // This is a simple array of the DataClass
    std::vector<DataClass> send_data(10);
    for (unsigned int i=0; i<send_data.size(); ++i)
    {
        send_data[i] = DataClass(i*std::pow(10, mpi_rank));
    };

    // Initialize recv_data array with the size equal to the number of
    // imported data. num_import() is calculated within the zcomm class
    std::vector<DataClass> recv_data(zcomm.num_import());

    // Perform communication of send_data and write it in recv_data
    zcomm.communicate(send_data.data(), recv_data.data());

    // Print out received data
    printf("Rank %d received: ", mpi_rank);
    for (int i=0; i<recv_data.size(); ++i)
        printf("%.1f ", recv_data[i].a);
    printf("\n");

    // Here, we will update recv_data and send the updated data
    // back to the owning rank using communicate_reverse
    for (unsigned int i=0; i<recv_data.size(); ++i)
        recv_data[i].a += 0.1*mpi_rank;

    // Create a container for the reverse communication data
    std::vector<DataClass> updated_recv_data(zcomm.num_export());

    // Perform reverse communication
    zcomm.communicate_reverse(recv_data.data(), updated_recv_data.data());

    // Print out the updated data
    printf("Rank %d updated: ", mpi_rank);
    for (int i=0; i<updated_recv_data.size(); ++i)
        printf("%.1f ", updated_recv_data[i].a);
    printf("\n");

    MPI_Finalize();

    return 0;

};

