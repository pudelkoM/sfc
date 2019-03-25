#pragma once

#include <tuple>
#include <string>

struct args {
    enum { ENCRYPT, DECRYPT } op;
    std::string pw;
};

std::pair<struct args, int> parse_args(int, char **);

int encrypt(std::istream &in, std::ostream &out, std::string &pw);

int decrypt(std::istream &in, std::ostream &out, std::string &pw);
