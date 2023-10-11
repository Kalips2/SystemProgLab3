#include <iostream>
#include <filesystem>
#include <fstream>
#include <regex>
#include <cassert>

#define TEST_FOLDER "C:\\Users\\User\\CLionProjects\\SystemProgLab3\\tests"
#define RESULT_FILE "C:\\Users\\User\\CLionProjects\\SystemProgLab3\\result.txt"

void processFilesInsideDir(const std::string &pathToDir);

void processFile(const std::string &path);

void writeResults(const std::string &path, const std::string &text);

std::string analyze(const std::string &text);

struct PatternWithName {
    std::regex pattern;
    std::string name;
};

int main() {
    processFilesInsideDir(TEST_FOLDER);
    return 0;
}

void processFilesInsideDir(const std::string &pathToDir) {
    for (const auto &entry: std::filesystem::directory_iterator(pathToDir)) {
        if (entry.is_directory()) {
            continue;
        }
        processFile(entry.path().string());
    }
}

void processFile(const std::string &path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return;
    }

    std::string c_code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    writeResults(path, analyze(c_code));
}

void writeResults(const std::string &path, const std::string &text) {
    std::ofstream result_file(RESULT_FILE, std::ios::app); // Append mode
    if (!result_file.is_open()) {
        std::cerr << "Output file opening failed" << std::endl;
        return;
    }

    result_file << "For C file - " << path << std::endl;
    result_file << text << std::endl;

    result_file.close();
}

std::string analyze(const std::string &text) {

    std::vector<PatternWithName> patterns = {
            {std::regex(R"(/.*|/\*.*?\*/)"), "Comment"},
            {std::regex(R"(#\w+)"), "Preprocessor Directive"},
            {std::regex(R"(\"(.*?)\")"), "String Literal"},
            {std::regex(R"(\'(.)\')"), "Character Literal"},
            {std::regex(R"(\d+\.\d+|\d+|\.\d+|0x[0-9a-fA-F]+)"), "Number"},
            {std::regex(R"(\b(int|float|return|if|else|while|for|printf|main|scanf)\b)"), "Keyword"},
            {std::regex(R"(\b[a-zA-Z_]\w*\b)"), "Identifier"},
            {std::regex(R"(==|!=|<=|>=|&&|\+\+|--|\+|-|\*|/|%|&|\|\^|~|<<|>>|<|>|!|=)"), "Operator"},
            {std::regex(R"(\{|\}|\(|\)|\[|\]|\;|\,|\.)"), "Delimiter"},
            {std::regex(R"(\s+)"), "Whitespace"},
    };
    std::string remainingCode = text;
    std::ostringstream result;

    while (!remainingCode.empty()) {
        std::smatch match;
        bool found = false;

        for (const auto& [regex, type] : patterns) {
            if (std::regex_search(remainingCode, match, regex)) {
                if(match.position() == 0){
                    // Не звертаємо уваги на пробіли та таби.
                    if (type != "Whitespace") {
                        result << type + " - " << match[0] << "\n";
                    }
                    remainingCode = match.suffix().str();
                    found = true;
                    break;
                }
            }
        }

        // Знайшли те, що прочитати не можемо - exit.
        if (!found) {
            result << "Unrecognized sequence: " << remainingCode + "\n";
            break;
        }
    }

    return result.str();
}