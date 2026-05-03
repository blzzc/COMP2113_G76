#include "Game.h"

#include <filesystem>
#include <string>

/**
 * Starts the Echoes of Time program.
 * Inputs: none.
 * Outputs: process exit code.
 */
int main(int argc, char* argv[]) {
    // default 先用 current dir, 比较保险.
    std::string executableDirectory = ".";

    if (argc > 0 && argv[0] != NULL) {
        // 想拿到 executable 在哪里, 这样读 maps/data path 会稳一点.
        std::filesystem::path executablePath(argv[0]);

        if (executablePath.has_parent_path()) {
            // basically store the folder path, later Game 再自己 resolve files.
            executableDirectory = std::filesystem::absolute(executablePath).parent_path().string();
        }
    }

    // main 很薄, 真正 logic 都丢给 Game class.
    Game game(executableDirectory);
    game.run();
    return 0;
}
