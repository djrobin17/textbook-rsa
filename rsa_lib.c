// Description: Implementation of functions for generating RSA keys, encrypting and decrypting messages
#include "rsa_lib.h"
#include <stdio.h>
#include <gmp.h>
#include <stdlib.h>
#include <time.h>

// Helper functions

// Generate a random prime number of the specified bit size
void generate_random_prime(mpz_t prime, int bit_size)
{
    // Seed the random state with the current time (not a secure way)
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, (unsigned long)time(NULL));

    do
    {
        mpz_urandomb(prime, state, bit_size);      // Generate a random number of the specified bit size
        mpz_nextprime(prime, prime);               // Find the next prime number after the random number
    } while (mpz_sizeinbase(prime, 2) < bit_size); // Repeat until the prime number is the correct bit size

    gmp_randclear(state);
}

// encode a string to an mpz_t number
void encodeBase256(mpz_t number, const char *string)
{
    mpz_set_ui(number, 0);
    for (size_t i = 0; string[i] != '\0'; i++)
    {
        mpz_mul_ui(number, number, 256);                      // Multiply by 256 for the next byte
        mpz_add_ui(number, number, (unsigned char)string[i]); // Add the character value
    }
}

// decode an mpz_t number to a string
void decodeBase256(char *string, const mpz_t number)
{
    mpz_t temp;
    mpz_init(temp);
    mpz_set(temp, number);

    size_t i = 0;
    while (mpz_sgn(temp) > 0)
    {
        string[i++] = (char)mpz_fdiv_q_ui(temp, temp, 256); // Divide by 256 and get the remainder
    }
    string[i] = '\0'; // Null-terminate the string

    // Reverse the string in-place
    for (size_t start = 0, end = i - 1; start < end; start++, end--)
    {
        char tempChar = string[start];
        string[start] = string[end];
        string[end] = tempChar;
    }

    mpz_clear(temp);
}

// Crypto functions

// Generate RSA keys and write them to files in the specified directory
void generate_rsa_keys(const char *keys_path)
{
    RSAKey key;
    mpz_t p, q, phi;
    mpz_inits(key.n, key.e, key.d, p, q, phi, NULL);

    // Generate two random prime numbers p and q of half the keysize (512 bits each for a 1024 bit key)
    generate_random_prime(p, KEY_SIZE / 2);
    generate_random_prime(q, KEY_SIZE / 2);
    while (mpz_cmp(p, q) == 0)
    {
        generate_random_prime(q, KEY_SIZE / 2);
    }

    // Calculate n = p * q
    mpz_mul(key.n, p, q);

    // Calculate phi = (p-1)*(q-1)
    mpz_sub_ui(p, p, 1);
    mpz_sub_ui(q, q, 1);
    mpz_mul(phi, p, q);

    // Choose e, usually a small odd integer that is co-prime with phi, using the common default value 65537 here
    mpz_set_ui(key.e, 65537);

    // Calculate d, the modular inverse of e modulo phi
    mpz_invert(key.d, key.e, phi);

    // Construct file paths for public and private keys
    char public_key_path[1024];
    char private_key_path[1024];
    snprintf(public_key_path, sizeof(public_key_path), "%s/public.key", keys_path);
    snprintf(private_key_path, sizeof(private_key_path), "%s/private.key", keys_path);

    // Convert mpz_t values to strings
    char *n_str = mpz_get_str(NULL, 10, key.n);
    char *e_str = mpz_get_str(NULL, 10, key.e);
    char *d_str = mpz_get_str(NULL, 10, key.d);

    // Write n and e to public key file
    FILE *pub_file = fopen(public_key_path, "w");
    if (pub_file != NULL)
    {
        fprintf(pub_file, "n:%s\ne:%s", n_str, e_str);
        fclose(pub_file);
    }
    else
    {
        fprintf(stderr, "Error: Unable to open file for writing public key: %s\n", public_key_path);
    }

    // Write n and d to private key file
    FILE *priv_file = fopen(private_key_path, "w");
    if (priv_file != NULL)
    {
        fprintf(priv_file, "n:%s\nd:%s", n_str, d_str);
        fclose(priv_file);
    }
    else
    {
        fprintf(stderr, "Error: Unable to open file for writing private key: %s\n", private_key_path);
    }

    puts("RSA keys generated successfully and written to files public.key and private.key");

    // Free allocated resources
    free(n_str);
    free(e_str);
    free(d_str);
    mpz_clears(p, q, phi, key.n, key.e, key.d, NULL);
}

// Encrypt a message using the specified public key and write the ciphertext to a file
void encrypt_message(const char *public_key_path, const char *message_str)
{
    mpz_t n, e, message, ciphertext;
    char buffer[KEY_SIZE / 2]; // Buffer for reading from file

    mpz_inits(n, e, message, ciphertext, NULL);

    // Encode the message string to an mpz_t number
    encodeBase256(message, message_str);

    // Check if the message size exceeds the key size
    if (mpz_sizeinbase(message, 2) > KEY_SIZE)
    {
        fprintf(stderr, "Error: Message size exceeds key size. Please use a smaller message or a larger key size (at least %zu bits)\n", mpz_sizeinbase(message, 2));
        mpz_clears(n, e, message, ciphertext, NULL);
        return;
    }

    // Open the public key file
    FILE *pub_file = fopen(public_key_path, "r");
    if (pub_file == NULL)
    {
        fprintf(stderr, "Error: Unable to open public key file: %s\n", public_key_path);
        mpz_clears(n, e, message, ciphertext, NULL);
        return;
    }

    // Read n and e from the file
    if (fgets(buffer, sizeof(buffer), pub_file) != NULL)
    {
        char *n_str = buffer + 2; // Skip "n:"
        mpz_set_str(n, n_str, 10);
    }
    if (fgets(buffer, sizeof(buffer), pub_file) != NULL)
    {
        char *e_str = buffer + 2; // Skip "e:"
        mpz_set_str(e, e_str, 10);
    }

    fclose(pub_file);

    // Perform RSA encryption: ciphertext = message^e mod n
    mpz_powm(ciphertext, message, e, n);

    // Convert the mpz_t ciphertext to a string
    char *ciphertext_str = mpz_get_str(NULL, 10, ciphertext);

    // Open file to write the encrypted message
    FILE *output_file = fopen("ciphertext_file", "w");
    if (output_file == NULL)
    {
        fprintf(stderr, "Error: Unable to open file for writing encrypted message\n");
        mpz_clears(n, e, message, ciphertext, NULL);
        free(ciphertext_str);
        return;
    }

    // Write the encrypted message string to the file
    fprintf(output_file, "c:%s", ciphertext_str);
    fclose(output_file);
    puts("Encrypted message successfully and written to ciphertext_file");

    // Free allocated resources
    free(ciphertext_str);
    mpz_clears(n, e, message, ciphertext, NULL);
}

// Decrypt a message using the specified private key and print the plaintext as output
void decrypt_message(const char *private_key_path, const char *ciphertext_file_path)
{
    mpz_t n, d, ciphertext, message;
    char buffer[KEY_SIZE / 2]; // Buffer for reading from file

    mpz_inits(n, d, ciphertext, message, NULL);

    // Read the ciphertext from file
    FILE *ciphertext_file = fopen(ciphertext_file_path, "r");
    if (ciphertext_file == NULL)
    {
        fprintf(stderr, "Error: Unable to open ciphertext file: %s\n", ciphertext_file_path);
        mpz_clears(n, d, ciphertext, message, NULL);
        return;
    }
    if (fgets(buffer, sizeof(buffer), ciphertext_file) != NULL)
    {
        mpz_set_str(ciphertext, buffer + 2, 10); // Skip "c:"
    }
    fclose(ciphertext_file);

    // Open the private key file
    FILE *priv_file = fopen(private_key_path, "r");
    if (priv_file == NULL)
    {
        fprintf(stderr, "Error: Unable to open private key file: %s\n", private_key_path);
        mpz_clears(n, d, ciphertext, message, NULL);
        return;
    }

    // Read n and d from the file
    if (fgets(buffer, sizeof(buffer), priv_file) != NULL)
    {
        char *n_str = buffer + 2; // Skip "n:"
        mpz_set_str(n, n_str, 10);
    }
    if (fgets(buffer, sizeof(buffer), priv_file) != NULL)
    {
        char *d_str = buffer + 2; // Skip "d:"
        mpz_set_str(d, d_str, 10);
    }
    fclose(priv_file);

    // Perform RSA decryption: message = ciphertext^d mod n
    mpz_powm(message, ciphertext, d, n);

    // Decode the message from an mpz_t number to a string
    char message_str[1024];
    decodeBase256(message_str, message);

    // Output the decrypted message
    printf("Decrypted message: %s\n", message_str);

    // Free allocated resources
    mpz_clears(n, d, ciphertext, message, NULL);
}