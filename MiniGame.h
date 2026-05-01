#ifndef MINIGAME_H
#define MINIGAME_H

#include "Common.h"

#include <random>
#include <string>

/**
 * Stores the result of one time-trial mini-game.
 * Inputs: none.
 * Outputs: a simple data structure for success, penalty, and summary text.
 */
struct MiniGameResult {
    std::string name;
    bool won;
    int timePenaltySeconds;
    std::string summary;
};

/**
 * Manages the four random terminal mini-games used in Time Trial mode.
 * Inputs: none.
 * Outputs: a mini-game manager object.
 */
class MiniGameManager {
private:
    std::mt19937 randomEngine_;

    /**
     * Reads one line of input from the player.
     * Inputs: none.
     * Outputs: full input line.
     */
    std::string readLine() const;

    /**
     * Clears the terminal by printing blank lines.
     * Inputs: none.
     * Outputs: none.
     */
    void clearScreen() const;

    /**
     * Prints a mini-game title banner.
     * Inputs: game title text.
     * Outputs: none.
     */
    void printHeader(const std::string& title) const;

    /**
     * Returns the time penalty for the selected difficulty and result.
     * Inputs: difficulty and whether the player won.
     * Outputs: penalty in seconds.
     */
    int penaltyForResult(Difficulty difficulty, bool won) const;

    /**
     * Runs a number-guessing mini-game.
     * Inputs: difficulty.
     * Outputs: mini-game result.
     */
    MiniGameResult playNumberGuess(Difficulty difficulty);

    /**
     * Runs a math quiz mini-game.
     * Inputs: difficulty.
     * Outputs: mini-game result.
     */
    MiniGameResult playMathQuiz(Difficulty difficulty);

    /**
     * Runs a memory-sequence mini-game.
     * Inputs: difficulty.
     * Outputs: mini-game result.
     */
    MiniGameResult playMemoryTrial(Difficulty difficulty);

    /**
     * Runs a rock-paper-scissors mini-game.
     * Inputs: difficulty.
     * Outputs: mini-game result.
     */
    MiniGameResult playRockPaperScissors(Difficulty difficulty);

public:
    /**
     * Creates a mini-game manager with a seeded random engine.
     * Inputs: none.
     * Outputs: initialized mini-game manager.
     */
    MiniGameManager();

    /**
     * Launches one of the four mini-games at random.
     * Inputs: current difficulty.
     * Outputs: mini-game result.
     */
    MiniGameResult playRandomMiniGame(Difficulty difficulty);
};

#endif
