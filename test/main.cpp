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
    MPI_Init(&argc, &argv);
    MPI_Comm comm = MPI_COMM_WORLD;
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // This is a nested list where each entry
    // is a list of array index to be sent to
    // the specific rank. export_ids[this_rank] = {} always
    std::vector<std::vector<int>> export_procs_ids(size);

    if (rank == 0)
        export_procs_ids = { {}, {2}, {7} };
    else if (rank == 1)
        export_procs_ids = { {}, {}, {3} };
    else
        export_procs_ids = { {0}, {6}, {} };

    zcomm::ZCommWrapper<DataClass> zcomm = zcomm::ZCommWrapper<DataClass>(
            comm, export_procs_ids, 3);

    std::vector<DataClass> send_data(10);
    for (unsigned int i=0; i<send_data.size(); ++i)
    {
        send_data[i] = DataClass(i*std::pow(10, rank));
    };

    std::vector<DataClass> recv_data(zcomm.num_import());
    zcomm.communicate(send_data.data(), recv_data.data());

    for (int r=0; r<size; ++r)
    {
        if (rank == r)
        {
            printf("Rank %d: ", r);
            for (int i=0; i<zcomm.num_import(); ++i)
                printf("%d ", (int)recv_data[i].a);
            printf("\n");
        };
        MPI_Barrier(comm);
    }

    for (unsigned int i=0; i<recv_data.size(); ++i)
        recv_data[i].a += 0.1;
    std::vector<DataClass> reverse_recv_data(zcomm.num_export());
    zcomm.communicate_reverse(recv_data.data(), reverse_recv_data.data());

    for (int r=0; r<size; ++r)
    {
        if (rank == r)
        {
            printf("Rank %d: ", r);
            for (int i=0; i<zcomm.num_export(); ++i)
                printf("%f ", reverse_recv_data[i].a);
            printf("\n");
        };
        MPI_Barrier(comm);
    }

    MPI_Finalize();

    return 0;

};

