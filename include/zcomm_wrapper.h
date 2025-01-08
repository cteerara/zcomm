//
// Created by cteerara on 1/8/25.
//

#ifndef ZCOMM_ZCOMM_WRAPPER_H
#define ZCOMM_ZCOMM_WRAPPER_H

#include <iostream>
#include <mpi.h>
#include <vector>
#include <numeric>
#include <cstring>
#include "zoltan.h"
#include "zoltan_cpp.h"
#include <cmath>
#include <cassert>

namespace zcomm
{

template <typename T>
class ZComm
{
public:

    // ----------------------------------------------------------------------
    // Members
    // ----------------------------------------------------------------------


    // ----------------------------------------------------------------------
    // Constructor
    // ----------------------------------------------------------------------
    ZComm(MPI_Comm& _comm,
          const std::vector<std::vector<int>>& _export_ids_start,
          int _stride)
    {
        comm = _comm;
        stride_size = _stride;
        MPI_Comm_rank(comm, &mpi_rank);
        MPI_Comm_size(comm, &mpi_size);

        // Make sure export_ids_start is a nested list with size equal to the number of mpi procs
        assert(_export_ids_start.size() == mpi_size);

        nbyte = sizeof(T);
        set_export_data(_export_ids_start);
        plan.Create(nexport, export_procs.data(), comm, tag, &nimport);
    };

    // ----------------------------------------------------------------------
    // Methods
    // ----------------------------------------------------------------------
    void set_export_data(const std::vector<std::vector<int>>& _export_ids_start)
    {
        nexport = 0;
        for (int r=0; r<mpi_size; ++r)
            nexport += _export_ids_start[r].size() * stride_size;

        export_procs.resize(nexport);
        export_ids.resize(nexport);
        int count = 0;
        for (int r=0; r < mpi_size; ++r)
        {
            for (int i=0; i < _export_ids_start[r].size(); ++i)
            {
                for (int stride_step=0; stride_step < stride_size; ++stride_step)
                {
                    export_procs[count] = r;
                    export_ids[count] = _export_ids_start[r][i] + stride_step;
                    count++;
                }
            }
        }
    };
    // ----------------------------------------------------------------------

    T* get_export_data(const T* send_data)
    {
        T* export_data = new T[nexport];
        for (int i=0; i<nexport; ++i)
            export_data[i] = send_data[export_ids[i]];
        return export_data;
    };
    // ----------------------------------------------------------------------

    void communicate(const T* send_data, T* recv_data)
    {
        const T* export_data = get_export_data(send_data); // new export_data
        char* sendbuf = serialize(export_data, nexport); // new sendbuf
        char* recvbuf = new char[nimport*nbyte]; // new recvbuf

        plan.Do(tag, sendbuf, nbyte, recvbuf);
        deserialize(recvbuf, recv_data, nimport);

        delete[] sendbuf;
        delete[] recvbuf;
        delete[] export_data;
    };
    // ----------------------------------------------------------------------

    void communicate_reverse(const T* export_data, T* recv_data)
    {
        char* sendbuf = serialize(export_data, nimport); // new sendbuf
        char* recvbuf = new char[nexport*nbyte]; // new recvbuf

        plan.Do_Reverse(tag, sendbuf, nbyte, nullptr, recvbuf);
        deserialize(recvbuf, recv_data, nexport);

        delete[] sendbuf;
        delete[] recvbuf;
    };

    // ----------------------------------------------------------------------
    // Getters
    // ----------------------------------------------------------------------
    int num_export()
    {
        return nexport;
    };

    int num_import()
    {
        return nimport;
    };

    int stride()
    {
        return stride_size;
    };

private:

    // ----------------------------------------------------------------------
    // Members
    // ----------------------------------------------------------------------
    MPI_Comm comm;
    int mpi_rank;
    int mpi_size;
    int nbyte; // size of a single element in the vector to send
    int nexport;
    int nimport;
    int stride_size;

    std::vector<int> export_procs;
    std::vector<int> export_ids;
    int tag = 100;
    Zoltan_Comm plan;

    // ----------------------------------------------------------------------
    // Methods
    // ----------------------------------------------------------------------
    char* serialize(const T* data, int n)
    {
        char* buffer = new char[n*nbyte];
        std::memcpy(buffer, data, n*nbyte);
        return buffer;
    };
    // ----------------------------------------------------------------------

    void deserialize(const char* buffer, T* data, int n)
    {
        for (unsigned int i=0; i<n; ++i)
        {
            const char* p = buffer + i*nbyte;
            std::memcpy(&data[i], p, nbyte);
        }
    };

};
// ----------------------------------------------------------------------

} // namespace zcomm



#endif //ZCOMM_ZCOMM_WRAPPER_H
