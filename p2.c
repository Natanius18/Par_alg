#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main() {
    long long sizes[] = {4, 5, 6, 7};
    int thread_counts[] = {1, 2, 4, 8};

    for (int s = 0; s < 4; s++) {
        long long n = pow(10, sizes[s]);
        int *a = malloc(sizeof(int) * n);

        // Заповнення масиву випадковими значеннями
        for (long long i = 0; i < n; i++) {
            a[i] = rand();
        }

        printf("\n=== n = %lld ===\n", n);

        for (int t = 0; t < 4; t++) {
            int thread_count = thread_counts[t];

            printf("Sorting started: threads = %d | progress [", thread_count);
            fflush(stdout); // Примусовий вивід повідомлення на екран
            // Копія початкового масиву для кожного запуску
            int *a_copy = malloc(sizeof(int) * n);
            for (long long i = 0; i < n; i++) a_copy[i] = a[i];

            double s_time = omp_get_wtime();

            // Кожна фаза сортування виконується у паралельному блоці
#pragma omp parallel num_threads(thread_count) default(none) shared(a_copy, n)
            {
                int i, tmp;
                for (int phase = 0; phase < n; phase++) {
                    // Вивід прогресу у відсотках (тільки з одного потоку)
#pragma omp master
                    if (phase % (n / 10) == 0) {
                        int percent = (int)((100.0 * phase) / n);
                        printf("%d%% ", percent);
                        fflush(stdout); // Примусовий вивід відсотків на екран
                    }

                    if (phase % 2 == 0) {
#pragma omp for
                        for (i = 1; i < n; i += 2) {
                            if (a_copy[i - 1] > a_copy[i]) {
                                tmp = a_copy[i - 1];
                                a_copy[i - 1] = a_copy[i];
                                a_copy[i] = tmp;
                            }
                        }
                    } else {
#pragma omp for
                        for (i = 1; i < n - 1; i += 2) {
                            if (a_copy[i] > a_copy[i + 1]) {
                                tmp = a_copy[i + 1];
                                a_copy[i + 1] = a_copy[i];
                                a_copy[i] = tmp;
                            }
                        }
                    }
                }
            }

            double f_time = omp_get_wtime();
            printf("100%%] | time = %.6f sec\n", f_time - s_time);
            free(a_copy);
        }
        free(a);
    }
    return 0;
}