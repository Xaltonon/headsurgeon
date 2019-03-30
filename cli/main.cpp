#include <filesystem>
#include <iostream>

#include <CLI/CLI.hpp>

#include "dmi.h"

namespace fs = std::filesystem;

int run_slice(std::string input, std::optional<std::string> output, bool force,
              bool recurse) {
    DMI dmi;

    if (!fs::exists(input)) {
        std::cerr << "Couldn't open file " << input << "\n";
        return 1;
    }

    try {
        dmi.load(input);
    } catch (DMIError &e) {
        std::cerr << "Error loading DMI: " << e.describe() << "\n";
        return 2;
    }

    fs::path outpath;
    if (output.has_value())
        outpath = output.value();
    else
        outpath = fs::path(input).stem();

    if (fs::exists(outpath) && !force) {
        std::cerr << "Cowardly refusing to overwrite existing files (use -f to "
                     "force)\n";
        return 3;
    }

    try {
        dmi.split(outpath, [](int total, int i, std::string name){
                               std::cout << "\r[" << std::setw(3) << i << "/"
                                         << std::setw(3) << total << "] "
                                         << std::setw(32) << name;
                               std::flush(std::cout);
                           });
        std::cout << std::setw(100) << "\rDone.\n";
    } catch (DMIError &e) {
        std::cerr << "Error splitting DMI: " << e.describe() << "\n";
        return 4;
    }

    return 0;
}

int run_ls(std::string input) {
    DMI dmi;

    if (!fs::exists(input)) {
        std::cerr << "Couldn't open file " << input << "\n";
        return 1;
    }

    try {
        dmi.load(input);
    } catch (DMIError &e) {
        std::cerr << "Error loading DMI: " << e.describe() << "\n";
        return 2;
    }

    std::cout << std::setw(8) << "width"
              << std::setw(8) << "height"
              << "\n";
    std::cout << std::setw(8) << dmi.width
              << std::setw(8) << dmi.height
              << "\n\n";

    std::cout << std::setw(32) << "name"
              << std::setw(5) << "dirs"
              << std::setw(8) << "frames"
              << "\n";

    for (auto &s : dmi.states) {
        std::cout << std::setw(32) << s.name
                  << std::setw(5) << s.dirs
                  << std::setw(8) << s.frames
                  << "\n";
    }

    return 0;
}

int main(int argc, char **argv) {
    int ret = 1;

    CLI::App app{"hsdmi - Manipulate BYOND DMIs"};

    CLI::App *ls =
        app.add_subcommand("ls", "List icon states in a DMI");

    std::string input;
    std::optional<std::string> output;
    bool recurse, force;

    ls->add_option("INPUT", input, "Input DMI");
    ls->callback([&]() { ret = run_ls(input); });

    CLI::App *slice =
        app.add_subcommand("slice", "Slice a DMI into a directory");

    slice->add_option("INPUT", input, "Input DMI")->required();
    slice->add_option("-o,--output", output, "Rename the output directory");
    slice->add_flag("-r,--recursive", recurse, "Recurse into directories");
    slice->add_flag("-f,--force", force, "Overwrite existing output");
    slice->callback([&]() { ret = run_slice(input, output, force, recurse); });

    app.require_subcommand();

    CLI11_PARSE(app, argc, argv);
    return ret;
}
