#include <stdio.h>
#include <omp.h>
#include <math.h>

// Функція для обчислення значення підінтегрального виразу
// f(x) = 4 / (1 + x^2) — це інтеграл від 0 до 1 для наближення числа π
double f(const double y) {
    return 4.0 / (1.0 + y * y);
}

void calculate(const int power) {
    double x;
    long long i;
    const long long n = pow(10, power);  // Кількість розбиттів (ітерацій)
    printf("\nn = %lld\n", n);
    const long double h = 1.0 / n;            // Ширина прямокутника (крок інтегрування)
    double sum = 0.0;
    // === ПОСЛІДОВНЕ ОБЧИСЛЕННЯ ===
    double s_time = omp_get_wtime(); // Старт часу
    for (i = 1; i <= n; i++) {
        x = h * (i - 0.5);            // Центр прямокутника
        sum += f(x);                 // Додавання площі прямокутника до суми
    }
    double pi = h * sum;             // Остаточне наближення до числа π
    double f_time = omp_get_wtime(); // Кінець часу
    printf("[SEQUENTIAL] pi = %.5f | error = %.15f | time = %.4f sec\n", pi, fabs(M_PI - pi), f_time - s_time);

    // === ПАРАЛЕЛЬНЕ ОБЧИСЛЕННЯ з 2 потоками ===
    s_time = omp_get_wtime();
    sum = 0.0;
#pragma omp parallel for private(x) shared(h) reduction(+:sum) num_threads(2)
    for (i = 1; i <= n; i++) {
        x = h * (i - 0.5);            // Центр прямокутника
        sum += f(x);                 // Додавання в потоці до спільної суми (через reduction)
    }
    pi = h * sum;
    f_time = omp_get_wtime();
    printf("[PARALLEL 2 THREADS] pi = %.5f | error = %.15f | time = %.4f sec\n", pi, fabs(M_PI - pi), f_time - s_time);

    // === ПАРАЛЕЛЬНЕ ОБЧИСЛЕННЯ з 4 потоками ===
    s_time = omp_get_wtime();
    sum = 0.0;
#pragma omp parallel for private(x) shared(h) reduction(+:sum) num_threads(4)
    for (i = 1; i <= n; i++) {
        x = h * (i - 0.5);
        sum += f(x);
    }
    pi = h * sum;
    f_time = omp_get_wtime();
    printf("[PARALLEL 4 THREADS] pi = %.5f | error = %.15f | time = %.4f sec\n", pi, fabs(M_PI - pi), f_time - s_time);

    // === ПАРАЛЕЛЬНЕ ОБЧИСЛЕННЯ з 8 потоками ===
    s_time = omp_get_wtime();
    sum = 0.0;
#pragma omp parallel for private(x) shared(h) reduction(+:sum) num_threads(8)
    for (i = 1; i <= n; i++) {
        x = h * (i - 0.5);
        sum += f(x);
    }
    pi = h * sum;
    f_time = omp_get_wtime();
    printf("[PARALLEL 8 THREADS] pi = %.5f | error = %.15f | time = %.4f sec\n", pi, fabs(M_PI - pi), f_time - s_time);
}

int main() {
    calculate(4);
    calculate(6);
    calculate(8);
    calculate(10);
    return 0;
}
