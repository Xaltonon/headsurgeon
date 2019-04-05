#include <filesystem>
#include <iostream>
#include <regex>

#include <CLI/CLI.hpp>

#include "codecs/webp.h"
#include "dmi.h"

namespace fs = std::filesystem;

void update_status(int total, int i, std::string name) {
    std::cout << "\r[" << std::setw(3) << i << "/" << std::setw(3) << total
              << "] " << std::setw(32) << name;
    std::flush(std::cout);
}

int run_slice(std::string input, std::optional<std::string> output,
              bool force) {
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
        dmi.split(outpath, update_status);
        std::cout << "\r[  DONE ] " << std::setw(32)
                  << fs::path(input).filename().string();
        std::flush(std::cout);
        std::cout << "\n";
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

    std::cout << fs::path(input).filename().string() << "\n";

    std::cout << std::setw(8) << "width" << std::setw(8) << "height"
              << "\n";
    std::cout << std::setw(8) << dmi.width << std::setw(8) << dmi.height
              << "\n\n";

    std::cout << std::setw(32) << "name" << std::setw(5) << "dirs"
              << std::setw(8) << "frames"
              << "\n";

    for (auto &s : dmi.states) {
        std::cout << std::setw(32) << s.name << std::setw(5) << s.dirs
                  << std::setw(8) << s.frames << "\n";
    }

    std::cout << "\n";

    return 0;
}

int run_join(std::string input, std::optional<std::string> output, bool force) {
    DMI dmi;

    if (!fs::exists(input) || !fs::is_directory(input)) {
        std::cerr << "Couldn't open directory " << input << "\n";
        return 1;
    }

    fs::path outpath;
    if (output.has_value())
        outpath = output.value();
    else
        outpath = (fs::path(input) / "")
                      .parent_path()
                      .stem()
                      .replace_extension("dmi");

    if (fs::exists(outpath) && !force) {
        std::cerr << "Cowardly refusing to overwrite existing files (use -f to "
                     "force)\n";
        return 3;
    }

    try {
        dmi.join(input, update_status);
        dmi.save(outpath);
        std::cout << "\r[  DONE ] " << std::setw(32)
                  << fs::path(outpath).filename().string();
        std::flush(std::cout);
        std::cout << "\n";
    } catch (DMIError &e) {
        std::cerr << "Error opening WebP: " << e.describe() << "\n";
        return 2;
    }

    return 0;
}

int run_search(std::string regex, std::vector<std::string> input) {
    std::regex rx{regex};

    for (auto &s : input) {
        for (auto &p : fs::recursive_directory_iterator(s)) {
            if (fs::is_regular_file(p) && p.path().extension() == ".dmi") {
                try {
                    DMI dmi;
                    std::vector<std::string *> matches;
                    dmi.load(p);
                    for (auto &i : dmi.states)
                        if (std::regex_search(i.name, rx))
                            matches.push_back(&i.name);
                    if (!matches.empty()) {
                        std::cout << fs::relative(p).string() << ": ";
                        for (auto &m : matches)
                            std::cout << *m << " ";
                        std::cout << "\n";
                    }
                } catch (const DMIError &e) {
                    // ignore errors, since it's not a DMI
                }
            }
        }
    }

    return 0;
}

int main(int argc, char **argv) {
    int ret = 1;

    CLI::App app{"hsdmi - Manipulate BYOND DMIs"};

    CLI::App *ls = app.add_subcommand("ls", "List icon states in a DMI");

    std::vector<std::string> input;
    std::optional<std::string> output;
    bool force;

    ls->add_option("INPUT", input, "Input DMI(s)")->required();
    ls->callback([&]() {
        for (auto &s : input)
            ret = run_ls(s);
    });

    CLI::App *slice =
        app.add_subcommand("slice", "Slice a DMI into a directory");

    slice->add_option("INPUT", input, "Input DMI")->required();
    slice->add_option("-o,--output", output, "Rename the output directory");
    slice->add_flag("-f,--force", force, "Overwrite existing output");
    slice->callback([&]() {
        for (auto &s : input)
            ret = run_slice(s, output, force);
    });

    CLI::App *join = app.add_subcommand("join", "Join a directory into a DMI");

    join->add_option("INPUT", input, "Input directory")->required();
    join->add_option("-o,--output", output, "Rename the output file");
    join->add_flag("-f,--force", force, "Overwrite existing output");
    join->callback([&]() {
        for (auto &s : input)
            ret = run_join(s, output, force);
    });

    CLI::App *search = app.add_subcommand(
        "search", "Search a directory of DMIs for an icon state");

    std::string regex;
    search->add_option("REGEX", regex, "Regex to search with")->required();
    search->add_option("INPUT", input, "Directories to search")->required();
    search->callback([&]() { run_search(regex, input); });

    app.require_subcommand();

    CLI11_PARSE(app, argc, argv);
    return ret;
}
