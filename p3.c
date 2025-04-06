#include <stdio.h>
#include <mpi.h>
#define N 16

int main(int argc, char **argv) {
    int a[N] = {16, 24, 100, 100, 40, 20, 80, 80, 32, 200, 16, 24, 32, 44, 32, 32};
    MPI_Status status;
    MPI_Comm commCube;
    int myrank, rank;
    int ndims = 3;
    int dims[3] = {2, 2, 2};
    int periods[3] = {1, 1, 1};
    int reorder = 0;
    int coords[3], coords0[3];
    int i, j, ku, v1, v2;

    MPI_Init(&argc, &argv);
    MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, periods, reorder, &commCube);
    MPI_Comm_rank(commCube, &myrank);

    v1 = a[2 * myrank];
    v2 = a[2 * myrank + 1];
    MPI_Cart_coords(commCube, myrank, ndims, coords);
    i = 0;
    while ((i < ndims) && (coords[i] == 0))i++;
    ku = i + 1;
    for (i = 1; i < ku; i++) {
        while (v1 != v2)
            if (v1 > v2)v1 -= v2;
            else v2 -= v1;
        for (j = 0; j < ndims; j++)coords0[j] = coords[j];
        coords0[i - 1] = (coords0[i - 1] + 1) % 2;
        MPI_Cart_rank(commCube, coords0, &rank);
        MPI_Recv(&v2, 1,MPI_INT, rank, 99, commCube, &status);
    }
    while (v1 != v2)
        if (v1 > v2)v1 -= v2;
        else v2 -= v1;
    if (ku <= ndims) {
        for (j = 0; j < ndims; j++)coords0[j] = coords[j];
        coords0[i - 1] = (coords0[i - 1] + 1) % 2;
        MPI_Cart_rank(commCube, coords0, &rank);
        MPI_Send(&v1, 1,MPI_INT, rank, 99, commCube);
    } else
        printf("NOD = %d\n", v1);

    MPI_Finalize();
}
