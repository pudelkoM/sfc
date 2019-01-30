#include <csignal>
#include <iostream>

int main() {
    char buffer[256];
    std::cin.read(buffer, 256);
    auto c = std::cin.gcount();
    if (c >= 3) {
        std::string in{buffer, static_cast<unsigned long>(c)};
        if (in[0] == 'a') {
            if (in[1] == 'b') {
                if (in == "abc") {
                    std::raise(SIGABRT);
                }
            }
        }
    }
}