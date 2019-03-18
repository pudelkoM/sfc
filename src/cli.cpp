#include <CLI11.hpp>
#include <string>
#include <utility>

struct args {
    enum { ENCRYPT, DECRYPT } op;
    std::string pw;
};

std::pair<struct args, int> parse_args(int argc, char *argv[]) {
    CLI::App app{"Simple File Cryptor"};

    // Supply password on the cli
    std::string pw;
    auto pw_flag_option = app.add_option("--pw", pw, "Read password from cli");

    // Supply password as file
    std::string pw_file;
    auto pw_file_option =
        app.add_option("--pw-file", pw_file, "Read password from file")->check(CLI::ExistingFile);

    pw_file_option->excludes(pw_flag_option);
    pw_flag_option->excludes(pw_file_option);

    // Subcommands
    app.require_subcommand();

    auto enc_sub = app.add_subcommand("e", "Encrypt a file");
    auto dec_sub = app.add_subcommand("d", "Decrypt a file");

    // Expanded form of CLI11_PARSE(app, argc, argv);
    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        auto err = app.exit(e); 
        return {{}, err ? err : 1}; // "--help" is considered an error, make sure it's passed as code !=0
    }

    // Ensure we got either flag
    if (app.count("--pw") == 0 && app.count("--pw-file") == 0) {
        auto e = CLI::RequiredError("Either '--pw' or '--pw-file'");
        return {{}, app.exit(e)};
    }

    // Read pw from file
    if (app.count("--pw-file")) {
        try {
            pw.resize(4096);
            auto c = std::ifstream(pw_file, std::ios::binary).read(pw.data(), pw.size()).gcount();
            pw.resize(c);
        } catch (std::ios::failure &f) {
            auto e = CLI::FileError(pw_file);
            return {{}, app.exit(e)};
        }
    }

    struct args t = {enc_sub->parsed() ? args::ENCRYPT : args::DECRYPT, pw};
    return {t, EXIT_SUCCESS};
}
