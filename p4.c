#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <tgmath.h>
#include <time.h>


int *my_keys;

void Bubble_sort(int a[] /* in/out */ ,
                 int n /* in */) {
    int list_length, i, temp;
    for (list_length = n; list_length >= 2; list_length--)
        for (i = 0; i < list_length - 1; i++)
            if (a[i] > a[i + 1]) {
                temp = a[i];
                a[i] = a[i + 1];
                a[i + 1] = temp;
            }
} // Bubble sort

void Merge_smaller(
    int my_keys[], /* in/out  */
    int recv_keys[], /* in      */
    int temp_keys[], /* scratch */
    int local_n /* = n/p, in*/) {
    int m_i, r_i, t_i;
    m_i = r_i = t_i = 0;
    while (t_i < local_n) {
        if (my_keys[m_i] <= recv_keys[r_i]) {
            temp_keys[t_i] = my_keys[m_i];
            t_i++;
            m_i++;
        } else {
            temp_keys[t_i] = recv_keys[r_i];
            t_i++;
            r_i++;
        }
    }
    for (m_i = 0; m_i < local_n; m_i++)
        my_keys[m_i] = temp_keys[m_i];
} /* Merge_low */


void Merge_larger(
    int my_keys[], /* in/out  */
    int recv_keys[], /* in      */
    int temp_keys[], /* scratch */
    int local_n /* = n/p, in*/) {
    int m_i, r_i, t_i;
    m_i = r_i = t_i = local_n - 1;
    while (t_i >= 0) {
        if (my_keys[m_i] >= recv_keys[r_i]) {
            temp_keys[t_i] = my_keys[m_i];
            t_i--;
            m_i--;
        } else {
            temp_keys[t_i] = recv_keys[r_i];
            t_i--;
            r_i--;
        }
    }
    for (m_i = 0; m_i < local_n; m_i++)
        my_keys[m_i] = temp_keys[m_i];
} /* Merge_low */

int Compute_partner(int phase, int myrank, int comm_sz) {
    int partner;
    if (phase % 2 == 0) /* Even phase */
        if (myrank % 2 != 0) /* Odd rank */
            partner = myrank - 1;
        else /* Even rank */
            partner = myrank + 1;
    else /* Odd phase */
        if (myrank % 2 != 0) /* Odd rank */
            partner = myrank + 1;
        else /* Even rank */
            partner = myrank - 1;
    if (partner == -1 || partner == comm_sz)
        partner = MPI_PROC_NULL;
    return partner;
}


int main(int argc, char **argv) {
    int size, ns;
    int my_rank, comm_sz;
    int phase, partner;
    MPI_Status status;


    int power = atoi(argv[1]);
    long long n = pow(10, power);
    int *a = malloc(sizeof(int) * n);


    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    // Заповнення масиву випадковими значеннями
    srand(time(nullptr) + my_rank);
    for (long long i = 0; i < n; i++) {
        a[i] = rand();
    }
    printf("\n=== n = 10^%d ===\n", power);
    double start = MPI_Wtime();
    size = n / comm_sz;
    ns = size * my_rank;
    int *recv_keys = malloc(size * sizeof(int));
    int *temp_keys = malloc(size * sizeof(int));
    my_keys = &a[ns];

    // Sort_local_keys;
    Bubble_sort(my_keys, size);

    for (phase = 0; phase < comm_sz; phase++) {
        partner = Compute_partner(phase, my_rank, comm_sz);
        if (partner != MPI_PROC_NULL) {
            // Send my keys to partner; Receive keys from partner;
            MPI_Sendrecv(my_keys, size, MPI_INT, partner, 0,
                         recv_keys, size, MPI_INT, partner, 0, MPI_COMM_WORLD,
                         &status);
            if (my_rank < partner) //  Keep_smaller_keys;
                Merge_smaller(my_keys, recv_keys, temp_keys, size);
            else //  Keep_larger_keys;
                Merge_larger(my_keys, recv_keys, temp_keys, size);
        }
    }
    printf("my_rank = %d; my_size = %d\n", my_rank, size);
    const double end = MPI_Wtime();
    if (my_rank == 0) {
        printf("Elapsed time: %f seconds\n", end - start);
    }


    MPI_Finalize();
    return 0;
}
