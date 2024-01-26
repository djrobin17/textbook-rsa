# Textbook RSA Implementation

This repository contains a C library for performing Textbook RSA operations, as well as a Python CLI application that interfaces with the library.

## C Library (rsa_lib)

The C library consists of the following files:

- `rsa_lib.c`: Contains the implementation of the RSA operations.
- `rsa_lib.h`: Contains the function declarations and necessary data structures.

The C library provides the following crypto operations:

- `generate_rsa_keys`: Generates an RSA key pair and saves the keys to the specified directory.
- `encrypt_message`: Encrypts a message using the RSA public key.
- `decrypt_message`: Decrypts a message using the RSA private key.

## Python CLI Application (rsa_cli)

The Python CLI application (`rsa_cli.py`) provides a command-line interface to interact with the C library.

It uses the `ctypes` library to load the `rsa_lib` shared library and define the argument and return types of the functions. This integration allows the Python script to call functions defined in the C library.

The application also uses `argparse` library to implement command-line argument parsing support.

## Build Instructions

**Requirements**: You need to have `gcc` and `make` installed. To build the shared C library, simply execute the following commands:

```
  git clone https://github.com/djrobin17/textbook-rsa
  cd textbook-rsa
  make
```
The Makefile contains instructions to compile a shared library i.e `rsa_lib.so` or `rsa_lib.dll` according to the OS running the program using gcc with warning flags and linking the gmp library:

#### For Unix/Linux
```
  gcc -Wall -fPIC -shared -o rsa_lib.so rsa_lib.c -lgmp
```

#### For Windows
```
  gcc -Wall -fPIC -shared -o rsa_lib.dll rsa_lib.c -lgmp
```

The script `rsa_cli.py` does not require additional setup if Python3 is installed.

## Usage

To use the Python CLI application, you can run the following commands as explained with examples in the table below:

| Option            | Description                                                       | Example                                                                                        |
| ----------------- | ------------------------------------------------------------------| ---------------------------------------------------------------------------------------------- |
| `--generate-keys` | Generates RSA key pair and saves them to the specified directory. | `python rsa_cli.py --generate-keys --key-path .`                                               |
| `--encrypt`       | Encrypts message using a specified RSA public key.                | `python rsa_cli.py --encrypt --public-key ./public.key --message "Hello World"`                |
| `--decrypt`       | Decrypts message using a specified RSA private key.               | `python rsa_cli.py --decrypt --private-key ./private.key --encrypted-message ciphertext_file` |
| `--help`          | Shows the help message.                                           | `python rsa_cli.py --help`                                                                     |

## Notes

1. Textbook RSA: 
   - No padding or constant time math operations.
  
   - GMP library functions to perform modular, exponential and arithmetic computations, conversions and generation of random prime numbers.
  
   - Uses time(NULL) as random seed for demo purposes, but this is **not** a cryptographically secure way (dev/random and /dev/urandom in Unix/Linux or BCryptGenRandom in Windows are some of the more secure sources of randomness).
  
2. Developed to work across Unix/Linux/Windows operating systems.
   
3. Sample format of files generated:
   - public.key

    ```
    n:109982097212020196876851931132403084567442818845147659742988097316796125947647410763756318120419996513200079769467275725131301800232043188659525877021444107181366159255831838790581831868057382886025490427089931664422533890075078726420094576023819617975855416043650483180786937107301765616175172581759303109823
    e:65537
    ```
   - private.key
  
    ```
    n:109982097212020196876851931132403084567442818845147659742988097316796125947647410763756318120419996513200079769467275725131301800232043188659525877021444107181366159255831838790581831868057382886025490427089931664422533890075078726420094576023819617975855416043650483180786937107301765616175172581759303109823
    d:98917935487820902462870748259278993801110052550595597233481703590967405402415871645922950627737557020828928726238294403804560070080680435667593775960938099349503423866400724958947573872514850981001031861558478034097690809036291760721900099387383784110730731503888828368000353005173331929151650881531914925569
    ```
   - ciphertext_file
  
    ```
    c:97259515139845146573945044393237816098494200887421801513009056078561444509499054237851478089067732261382671267423240520718773081372170932739933112930621229986815004321246005462226919485493714055535643607002558723200152160563160670925678806172098630103236457500737588479472585828591304083537867724248561674145
    ```
