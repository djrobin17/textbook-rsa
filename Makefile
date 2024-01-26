# Description: Makefile for building the shared library for Textbook RSA implementation

# Compiler settings for gcc with warning flags and linking the gmp library
CC=gcc
CFLAGS=-Wall
LIBS=-lgmp

# Source and header files
SRC=rsa_lib.c
HDR=rsa_lib.h

# Determine the operating system for building the appropriate shared library
ifeq ($(OS),Windows_NT)
    # Windows settings
    LIB_OUT=rsa_lib.dll
    RM=del /Q
else
    # Unix/Linux settings
    CFLAGS+= -fPIC
    LIB_OUT=rsa_lib.so
    RM=rm -f
endif

# Build shared library
all: $(LIB_OUT)

$(LIB_OUT): $(SRC) $(HDR)
	$(CC) $(CFLAGS) -shared -o $(LIB_OUT) $(SRC) $(LIBS)

# Remove the shared library
clean:
	$(RM) $(LIB_OUT)
