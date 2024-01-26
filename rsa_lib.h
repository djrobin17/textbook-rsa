// Description: Header file for rsa_lib.c
#ifndef RSA_LIB_H
#define RSA_LIB_H

#include <gmp.h>
#define KEY_SIZE 1024 // increase this for more security

typedef struct
{
    mpz_t n; // modulus
    mpz_t e; // public exponent
    mpz_t d; // private exponent
} RSAKey;

void generate_rsa_keys(const char *keys_path);
void encrypt_message(const char *message_str, const char *public_key_path);
void decrypt_message(const char *ciphertext_file_path, const char *private_key_path);

#endif