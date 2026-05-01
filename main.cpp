#include "Game.h"

#include <filesystem>
#include <string>

/**
 * Starts the Echoes of Time program.
 * Inputs: none.
 * Outputs: process exit code.
 */
int main(int argc, char* argv[]) {
    std::string executableDirectory = ".";

    if (argc > 0 && argv[0] != NULL) {
        std::filesystem::path executablePath(argv[0]);

        if (executablePath.has_parent_path()) {
            executableDirectory = std::filesystem::absolute(executablePath).parent_path().string();
        }
    }

    Game game(executableDirectory);
    game.run();
    return 0;
}
