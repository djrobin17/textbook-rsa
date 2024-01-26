# Description: Python CLI interface script for shared RSA library
import argparse
import ctypes
import os

# Load the RSA library
if os.name == "nt":  # Windows
    rsa_lib = ctypes.CDLL("./rsa_lib.dll")
else:  # Unix/Linux
    rsa_lib = ctypes.CDLL("./rsa_lib.so")

# Define the argument and return types for the C functions
rsa_lib.generate_rsa_keys.argtypes = [ctypes.c_char_p]
rsa_lib.generate_rsa_keys.restype = None

rsa_lib.encrypt_message.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
rsa_lib.encrypt_message.restype = None

rsa_lib.decrypt_message.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
rsa_lib.decrypt_message.restype = None


# Generate RSA key pair
def generate_rsa_keys(key_path):
    rsa_lib.generate_rsa_keys(key_path.encode("utf-8"))


# Encrypt a message
def encrypt_message(public_key_path, message):
    rsa_lib.encrypt_message(public_key_path.encode("utf-8"), message.encode("utf-8"))


# Decrypt a message
def decrypt_message(private_key_path, encrypted_message_path):
    rsa_lib.decrypt_message(
        private_key_path.encode("utf-8"), encrypted_message_path.encode("utf-8")
    )


def main():
    # Create the argument parser
    parser = argparse.ArgumentParser(description="Textbook RSA CLI")

    # Add the supported arguments
    parser.add_argument(
        "--generate-keys", action="store_true", help="Generate RSA key pair"
    )
    parser.add_argument("--key-path", help="Path to save the generated key pair")
    parser.add_argument("--encrypt", action="store_true", help="Encrypt a message")
    parser.add_argument("--public-key", help="Path to the public key")
    parser.add_argument("--message", help="Message to encrypt")
    parser.add_argument("--decrypt", action="store_true", help="Decrypt a message")
    parser.add_argument("--private-key", help="Path to the private key")
    parser.add_argument("--encrypted-message", help="Path to the encrypted message")

    # Parse the command line arguments
    args = parser.parse_args()

    if args.generate_keys:
        if not args.key_path:
            parser.error("valid --key-path is required when generating keys")
        generate_rsa_keys(args.key_path)
    elif args.encrypt:
        if not args.public_key or not args.message:
            parser.error(
                "valid --public-key and non-empty --message are required when encrypting"
            )
        encrypt_message(args.public_key, args.message)
    elif args.decrypt:
        if not args.private_key or not args.encrypted_message:
            parser.error(
                "valid --private-key and --encrypted-message are required when decrypting"
            )
        decrypt_message(args.private_key, args.encrypted_message)
    else:
        parser.error("No operation specified (use -h for help)")


if __name__ == "__main__":
    main()
