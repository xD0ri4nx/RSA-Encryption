#include <stdio.h>
#include <gmp.h>
#include <sys/time.h>

#define NUM_MESSAGES 10000
#define REPEAT_DECRYPTION 10000 // Repeats to add load

int main() {
    mpz_t d, n;
    mpz_inits(d, n, NULL);

    
    mpz_set_str(d, "987654321987654321987654321987654321987654321987654321", 10);
    mpz_set_str(n, "999999999999999999999999999999999999999999999999999999999999999999999999", 10);

    mpz_t ciphertext, plaintext;
    mpz_inits(ciphertext, plaintext, NULL);

    struct timeval start, end;
    gettimeofday(&start, NULL);

    for (int i = 0; i < NUM_MESSAGES; i++) {
        char message[100];
        snprintf(message, sizeof(message), "%d", i + 1000000);
        mpz_set_str(ciphertext, message, 10);

        
        for (int j = 0; j < REPEAT_DECRYPTION; j++) {
            mpz_powm(plaintext, ciphertext, d, n);
        }

        gmp_printf("Plaintext %d: %Zd\n", i + 1, plaintext);
    }

    gettimeofday(&end, NULL);

    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    printf("Execution time: %f seconds\n", time_taken);

    mpz_clears(d, n, ciphertext, plaintext, NULL);

    return 0;
}
