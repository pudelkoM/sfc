# Simple File Cryptor

SFC is a simple CLI tool to encrypt and decrypt single files, using a state-of-the-art symmetric AEAD cipher.

## Usage

Encrypt a file:

```bash
sfc --pw="my secret" e < some_file > some_file_encrypted
cat some_file | sfc --pw="my secret" e > some_file_encrypted # unnecessary use of cat
```

Decrypt a file:

```bash
sfc --pw="my secret" d < some_file_encrypted > some_file
```

To prevent passwords from showing up in the shell history, you can also read the password from a file instead of the CLI:

```bash
sfc --pw-file=secret_file e < some_file > some_file_encrypted
```

## Installation

### Pre-compiled binary
SFC comes as a single-file statically linked executable for Linux and Mac.

<TODO: insert download URLs once CICD is set up>

### Build from Source

```bash
git clone https://github.com/pudelkoM/sfc.git
mkdir build && cd build
cmake ..
make
```

All dependencies are fetched and build by CMake.

## Design

### Goals

- Easy-to-use
- Secure
- Not awfully slow
- Low code complexity

### Non-Goals

- File packing, use ``tar``
- Versioning
- Backups
- Error correction (error detection works, of course)

### Dependencies

- [libhydrogen](https://github.com/jedisct1/libhydrogen)
- [CLI11](https://github.com/CLIUtils/CLI11)

## Internals

### File Definition

### Cipher