/*#include <stdio.h>
#include <gmp.h>
#include <pthread.h>
#include <sys/time.h>

#define NUM_MESSAGES 10000
#define REPEAT_DECRYPTION 10000
#define NUM_THREADS 8 // Adjust based on your CPU cores

typedef struct {
    int id;
    mpz_t d, n;
} ThreadData;

void* decrypt_messages(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    mpz_t ciphertext, plaintext;
    mpz_inits(ciphertext, plaintext, NULL);

    for (int i = data->id; i < NUM_MESSAGES; i += NUM_THREADS) {
        char message[100];
        snprintf(message, sizeof(message), "%d", i + 1000000);
        mpz_set_str(ciphertext, message, 10);

        for (int j = 0; j < REPEAT_DECRYPTION; j++) {
            mpz_powm(plaintext, ciphertext, data->d, data->n);
        }

        gmp_printf("Plaintext %d: %Zd\n", i + 1, plaintext);
    }

    mpz_clears(ciphertext, plaintext, NULL);
    pthread_exit(NULL);
}

int main() {
    mpz_t d, n;
    mpz_inits(d, n, NULL);

    mpz_set_str(d, "987654321987654321987654321987654321987654321987654321", 10);
    mpz_set_str(n, "999999999999999999999999999999999999999999999999999999999999999999999999", 10);

    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    struct timeval start, end;
    gettimeofday(&start, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].id = i;
        mpz_init_set(thread_data[i].d, d);
        mpz_init_set(thread_data[i].n, n);
        pthread_create(&threads[i], NULL, decrypt_messages, &thread_data[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        mpz_clears(thread_data[i].d, thread_data[i].n, NULL);
    }

    gettimeofday(&end, NULL);

    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    printf("Execution time: %f seconds\n", time_taken);

    mpz_clears(d, n, NULL);

    return 0;
}*/


#include <stdio.h>
#include <gmp.h>
#include <windows.h>

#define NUM_MESSAGES 10000
#define REPEAT_DECRYPTION 10000
#define NUM_THREADS 8 // Adjust based on your CPU cores

typedef struct {
    int id;
    mpz_t d, n;
} ThreadData;

DWORD WINAPI decrypt_messages(LPVOID arg) {
    ThreadData* data = (ThreadData*)arg;
    mpz_t ciphertext, plaintext;
    mpz_inits(ciphertext, plaintext, NULL);

    for (int i = data->id; i < NUM_MESSAGES; i += NUM_THREADS) {
        char message[100];
        snprintf(message, sizeof(message), "%d", i + 1000000);
        mpz_set_str(ciphertext, message, 10);

        for (int j = 0; j < REPEAT_DECRYPTION; j++) {
            mpz_powm(plaintext, ciphertext, data->d, data->n);
        }

        gmp_printf("Plaintext %d: %Zd\n", i + 1, plaintext);
    }

    mpz_clears(ciphertext, plaintext, NULL);
    return 0;
}

int main() {
    mpz_t d, n;
    mpz_inits(d, n, NULL);

    mpz_set_str(d, "987654321987654321987654321987654321987654321987654321", 10);
    mpz_set_str(n, "999999999999999999999999999999999999999999999999999999999999999999999999", 10);

    HANDLE threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    LARGE_INTEGER frequency, start, end;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].id = i;
        mpz_init_set(thread_data[i].d, d);
        mpz_init_set(thread_data[i].n, n);
        threads[i] = CreateThread(NULL, 0, decrypt_messages, &thread_data[i], 0, NULL);
        if (threads[i] == NULL) {
            printf("Error creating thread %d\n", i);
            return 1;
        }
    }

    WaitForMultipleObjects(NUM_THREADS, threads, TRUE, INFINITE);

    for (int i = 0; i < NUM_THREADS; i++) {
        CloseHandle(threads[i]);
        mpz_clears(thread_data[i].d, thread_data[i].n, NULL);
    }

    QueryPerformanceCounter(&end);
    auto time_taken = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
    printf("Execution time: %f seconds\n", time_taken);

    mpz_clears(d, n, NULL);

    return 0;
}




/*
#include <stdio.h>
#include <gmp.h>
#include <windows.h>
#include <stdlib.h>

#define NUM_MESSAGES 10000
#define REPEAT_DECRYPTION 10000
#define NUM_THREADS 8 // Adjust based on your CPU cores

typedef struct {
    int id;
    mpz_t d, n;
    mpz_t* results; // Pointer to shared results array
} ThreadData;

DWORD WINAPI decrypt_messages(LPVOID arg) {
    ThreadData* data = (ThreadData*)arg;
    mpz_t ciphertext, plaintext;
    mpz_inits(ciphertext, plaintext, NULL);

    for (int i = data->id; i < NUM_MESSAGES; i += NUM_THREADS) {
        char message[100];
        snprintf(message, sizeof(message), "%d", i + 1000000);
        mpz_set_str(ciphertext, message, 10);

        for (int j = 0; j < REPEAT_DECRYPTION; j++) {
            mpz_powm(plaintext, ciphertext, data->d, data->n);
        }

        // Store the result in the shared array
        mpz_set(data->results[i], plaintext);
    }

    mpz_clears(ciphertext, plaintext, NULL);
    return 0;
}

int main() {
    mpz_t d, n;
    mpz_inits(d, n, NULL);

    mpz_set_str(d, "987654321987654321987654321987654321987654321987654321", 10);
    mpz_set_str(n, "999999999999999999999999999999999999999999999999999999999999999999999999", 10);

    // Allocate array to store all results
    mpz_t* results = (mpz_t*)malloc(NUM_MESSAGES * sizeof(mpz_t));
    if (!results) {
        printf("Memory allocation failed\n");
        return 1;
    }
    for (int i = 0; i < NUM_MESSAGES; i++) {
        mpz_init(results[i]);
    }

    HANDLE threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    LARGE_INTEGER frequency, start, end;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].id = i;
        mpz_init_set(thread_data[i].d, d);
        mpz_init_set(thread_data[i].n, n);
        thread_data[i].results = results; // Pass pointer to results array
        threads[i] = CreateThread(NULL, 0, decrypt_messages, &thread_data[i], 0, NULL);
        if (threads[i] == NULL) {
            printf("Error creating thread %d\n", i);
            return 1;
        }
    }

    // Wait for all threads to complete
    WaitForMultipleObjects(NUM_THREADS, threads, TRUE, INFINITE);

    // Clean up threads
    for (int i = 0; i < NUM_THREADS; i++) {
        CloseHandle(threads[i]);
        mpz_clears(thread_data[i].d, thread_data[i].n, NULL);
    }

    QueryPerformanceCounter(&end);
    double time_taken = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;

    // Print results in order
    for (int i = 0; i < NUM_MESSAGES; i++) {
        gmp_printf("Plaintext %d: %Zd\n", i + 1, results[i]);
    }

    printf("Execution time: %f seconds\n", time_taken);

    // Clean up
    for (int i = 0; i < NUM_MESSAGES; i++) {
        mpz_clear(results[i]);
    }
    free(results);
    mpz_clears(d, n, NULL);

    return 0;
}
*/
