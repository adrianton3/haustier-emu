#include <cstring>
#include <fstream>
#include <streambuf>
#include <string>
#include <variant>
#include <vector>

#include "raylib.h"

#include "asm.h"

void run (const char* binaryFile) {
    InitWindow(640, 400, "haustier-emu");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_Q)) {
            break;
        }

        BeginDrawing();

        ClearBackground(WHITE);

        EndDrawing();
    }

    CloseWindow();
}

void tokenizeDebug (const std::string& source) {
    const auto tokensOrError = tokenize(source);

    if (const auto* tokens = std::get_if<std::vector<Token>>(&tokensOrError)) {
        for (const auto& token : *tokens) {
            printf("%s, ", stringify(token).c_str());
        }

        printf("\n");
    } else {
        const auto error = std::get<std::string>(tokensOrError);

        printf("error: %s\n", error.c_str());
    }
}

void assemblerDebug (const std::string& source) {
    const auto bytesOrError = assemble(source);

    if (const auto* bytes = std::get_if<std::vector<uint8_t>>(&bytesOrError)) {
        for (const auto byte : *bytes) {
            printf("0x%02x ", byte);
        }

        printf("\n");
    } else {
        const auto error = std::get<std::string>(bytesOrError);

        printf("error: %s\n", error.c_str());
    }
}

void printUsage (char* path) {
    fprintf(
        stderr,
        "Usage:\n"
        " %s <binary-file>\n"
        " %s help\n"
        " %s tokenize <source-file>\n"
        " %s compile <source-file>\n",
        path, path, path, path
    );
}

int main (int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    if (argc == 2) {
        if (strcmp(argv[1], "help") == 0) {
            printUsage(argv[0]);
        } else {
            run(argv[1]);
        }

        return 0;
    }

    if (argc == 3) {
        if (strcmp(argv[1], "tokenize") == 0) {
            std::ifstream file { argv[2] };
            const std::string source { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
            tokenizeDebug(source);
            return 0;
        }

        if (strcmp(argv[1], "compile") == 0) {
            std::ifstream file { argv[2] };
            const std::string source { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
            assemblerDebug(source);
            return 0;
        }
    }

    printUsage(argv[0]);
    return 1;
}