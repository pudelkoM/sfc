#include <array>
#include <iostream>

#include <hydrogen.h>

// manual forward declarations for cli.cpp
struct args {
    enum { ENCRYPT, DECRYPT } op;
    std::string pw;
};
std::pair<struct args, int> parse_args(int, char **);

const char CONTEXT[hydro_pwhash_CONTEXTBYTES] = "sfc";
const uint64_t OPSLIMIT = 10000;
const size_t MEMLIMIT = 0;
const uint8_t THREADS = 1;
const uint8_t master_key[hydro_pwhash_MASTERKEYBYTES] = {};

const size_t CHUNK_SIZE = 512;
const size_t PAYLOAD_SIZE = CHUNK_SIZE - hydro_secretbox_HEADERBYTES;

using key_buffer = std::array<uint8_t, hydro_secretbox_KEYBYTES>;

key_buffer generate_key(const std::string pw) {
    key_buffer derived_key;
    hydro_pwhash_deterministic(derived_key.data(), derived_key.size(), pw.c_str(), pw.length(),
                               CONTEXT, master_key, OPSLIMIT, MEMLIMIT, THREADS);
    return derived_key;
}

int check_stream(std::ios &stream) {
    if (stream.eof()) {
        // done
        return EXIT_SUCCESS;
    }
    if (stream.bad() || stream.fail()) {
        std::cerr << "io error" << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int encrypt(std::istream &in, std::ostream &out, const key_buffer &key) {
    std::array<uint8_t, PAYLOAD_SIZE> ibuffer;
    std::array<uint8_t, CHUNK_SIZE> obuffer;
    uint64_t chunk_nr = 0;
    do {
        in.read(reinterpret_cast<char *>(ibuffer.data()), ibuffer.size());
        std::streamsize ilen = in.gcount();
        hydro_secretbox_encrypt(obuffer.data(), ibuffer.data(), ilen, chunk_nr, CONTEXT,
                                key.data());
        chunk_nr++;
        size_t olen = ilen + hydro_secretbox_HEADERBYTES;
        out.write(reinterpret_cast<char *>(obuffer.data()), olen);
    } while (in.good() && out.good());

    in.sync();
    out.flush();
    return check_stream(in) || check_stream(out);
}

int decrypt(std::istream &in, std::ostream &out, const key_buffer &key) {
    std::array<uint8_t, CHUNK_SIZE> ibuffer;
    std::array<uint8_t, PAYLOAD_SIZE> obuffer;
    uint64_t chunk_nr = 0;
    do {
        in.read(reinterpret_cast<char *>(ibuffer.data()), ibuffer.size());
        std::streamsize ilen = in.gcount();
        int err = hydro_secretbox_decrypt(obuffer.data(), ibuffer.data(), ilen, chunk_nr, CONTEXT,
                                          key.data());
        if (err) {
            std::cerr << "decryption failed at block " << chunk_nr << std::endl;
            return err;
        }
        size_t olen = ilen - hydro_secretbox_HEADERBYTES;
        chunk_nr++;
        out.write(reinterpret_cast<char *>(obuffer.data()), olen);
    } while (in.good() && out.good());

    in.sync();
    out.flush();
    return check_stream(in) || check_stream(out);
}

int main(int argc, char *argv[]) {
    if (hydro_init() != 0) {
        std::cerr << "Failed to init hydro library" << std::endl;
        return EXIT_FAILURE;
    }

    struct args t;
    int err;
    std::tie(t, err) = parse_args(argc, argv);
    if (err)
        return err;

    auto key = generate_key(t.pw);
    hydro_memzero(t.pw.data(), t.pw.size());

    switch (t.op) {
    case args::ENCRYPT:
        return encrypt(std::cin, std::cout, key);
        break;
    case args::DECRYPT:
        return decrypt(std::cin, std::cout, key);
        break;
    default:
        return EXIT_FAILURE;
    }
}