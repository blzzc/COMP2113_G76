#include "MiniGame.h"

#include <cctype>
#include <ctime>
#include <iostream>
#include <sstream>

/**
 * Creates a mini-game manager with a seeded random engine.
 * Inputs: none.
 * Outputs: initialized mini-game manager.
 */
MiniGameManager::MiniGameManager() : randomEngine_(static_cast<unsigned int>(std::time(NULL)) + 97U) {
}

/**
 * Reads one line of input from the player.
 * Inputs: none.
 * Outputs: full input line.
 */
std::string MiniGameManager::readLine() const {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

/**
 * Clears the terminal by printing blank lines.
 * Inputs: none.
 * Outputs: none.
 */
void MiniGameManager::clearScreen() const {
    for (int line = 0; line < 20; ++line) {
        std::cout << '\n';
    }
}

/**
 * Prints a mini-game title banner.
 * Inputs: game title text.
 * Outputs: none.
 */
void MiniGameManager::printHeader(const std::string& title) const {
    clearScreen();
    std::cout << "==============================\n";
    std::cout << title << "\n";
    std::cout << "==============================\n";
}

/**
 * Returns the time penalty for the selected difficulty and result.
 * Inputs: difficulty and whether the player won.
 * Outputs: penalty in seconds.
 */
int MiniGameManager::penaltyForResult(Difficulty difficulty, bool won) const {
    if (difficulty == Difficulty::EASY) {
        return won ? 8 : 18;
    }

    if (difficulty == Difficulty::NORMAL) {
        return won ? 12 : 25;
    }

    return won ? 15 : 30;
}

/**
 * Runs a number-guessing mini-game.
 * Inputs: difficulty.
 * Outputs: mini-game result.
 */
MiniGameResult MiniGameManager::playNumberGuess(Difficulty difficulty) {
    printHeader("Mini Game: Number Guess");
    std::uniform_int_distribution<int> numberDistribution(1, 7);
    int target = numberDistribution(randomEngine_);
    int attempts = difficulty == Difficulty::HARD ? 3 : 4;

    std::cout << "Guess the hidden number between 1 and 7.\n";
    std::cout << "You have " << attempts << " attempts.\n\n";

    for (int attempt = 1; attempt <= attempts; ++attempt) {
        std::cout << "Attempt " << attempt << "/" << attempts << ": ";
        std::string line = readLine();
        std::istringstream parser(line);
        int guess = 0;

        if (!(parser >> guess)) {
            std::cout << "Invalid input.\n";
            continue;
        }

        if (guess == target) {
            MiniGameResult result = {"Number Guess", true, penaltyForResult(difficulty, true), "You guessed the correct number."};
            return result;
        }

        if (guess < target) {
            std::cout << "Too low.\n";
        } else {
            std::cout << "Too high.\n";
        }
    }

    MiniGameResult result = {"Number Guess", false, penaltyForResult(difficulty, false), "You failed to guess the number."};
    return result;
}

/**
 * Runs a math quiz mini-game.
 * Inputs: difficulty.
 * Outputs: mini-game result.
 */
MiniGameResult MiniGameManager::playMathQuiz(Difficulty difficulty) {
    printHeader("Mini Game: Math Quiz");
    std::uniform_int_distribution<int> valueDistribution(2, difficulty == Difficulty::HARD ? 25 : 15);
    int correctAnswers = 0;

    std::cout << "Answer 3 questions. Get at least 2 correct to win.\n\n";

    for (int question = 1; question <= 3; ++question) {
        int a = valueDistribution(randomEngine_);
        int b = valueDistribution(randomEngine_);
        bool useAddition = (question % 2 == 1);
        int correct = useAddition ? (a + b) : (a - b);

        std::cout << "Question " << question << ": " << a << (useAddition ? " + " : " - ") << b << " = ";
        std::string line = readLine();
        std::istringstream parser(line);
        int answer = 0;

        if (parser >> answer && answer == correct) {
            ++correctAnswers;
            std::cout << "Correct.\n";
        } else {
            std::cout << "Wrong. Correct answer: " << correct << "\n";
        }
    }

    bool won = correctAnswers >= 2;
    MiniGameResult result = {"Math Quiz", won, penaltyForResult(difficulty, won), won ? "You cleared the math quiz." : "You missed too many math questions."};
    return result;
}

/**
 * Runs a memory-sequence mini-game.
 * Inputs: difficulty.
 * Outputs: mini-game result.
 */
MiniGameResult MiniGameManager::playMemoryTrial(Difficulty difficulty) {
    printHeader("Mini Game: Memory Trial");
    std::uniform_int_distribution<int> digitDistribution(0, 9);
    int length = difficulty == Difficulty::HARD ? 5 : 4;
    std::string sequence;

    for (int index = 0; index < length; ++index) {
        sequence += static_cast<char>('0' + digitDistribution(randomEngine_));
    }

    std::cout << "Memorize this sequence: " << sequence << "\n";
    std::cout << "Press Enter when ready to continue.";
    readLine();

    clearScreen();
    std::cout << "Type the full sequence: ";
    std::string answer = readLine();
    bool won = answer == sequence;

    MiniGameResult result = {"Memory Trial", won, penaltyForResult(difficulty, won), won ? "You repeated the sequence perfectly." : "You forgot the sequence."};
    return result;
}

/**
 * Runs a rock-paper-scissors mini-game.
 * Inputs: difficulty.
 * Outputs: mini-game result.
 */
MiniGameResult MiniGameManager::playRockPaperScissors(Difficulty difficulty) {
    printHeader("Mini Game: Rock Paper Scissors");
    std::uniform_int_distribution<int> choiceDistribution(0, 2);
    int playerWins = 0;
    int cpuWins = 0;
    const char* names[3] = {"Rock", "Paper", "Scissors"};

    std::cout << "First to 2 wins. Enter r, p, or s.\n\n";

    while (playerWins < 2 && cpuWins < 2) {
        std::cout << "Your choice: ";
        std::string line = readLine();
        if (line.empty()) {
            continue;
        }

        char choice = static_cast<char>(std::tolower(static_cast<unsigned char>(line[0])));
        int playerChoice = -1;

        if (choice == 'r') {
            playerChoice = 0;
        } else if (choice == 'p') {
            playerChoice = 1;
        } else if (choice == 's') {
            playerChoice = 2;
        } else {
            std::cout << "Invalid choice.\n";
            continue;
        }

        int cpuChoice = choiceDistribution(randomEngine_);
        std::cout << "Computer chose " << names[cpuChoice] << ". ";

        if (playerChoice == cpuChoice) {
            std::cout << "Draw.\n";
        } else if ((playerChoice + 1) % 3 == cpuChoice) {
            ++cpuWins;
            std::cout << "Computer wins the round.\n";
        } else {
            ++playerWins;
            std::cout << "You win the round.\n";
        }

        std::cout << "Score: You " << playerWins << " - " << cpuWins << " Computer\n";
    }

    bool won = playerWins > cpuWins;
    MiniGameResult result = {"Rock Paper Scissors", won, penaltyForResult(difficulty, won), won ? "You beat the computer in rock-paper-scissors." : "The computer outplayed you."};
    return result;
}

/**
 * Launches one of the four mini-games at random.
 * Inputs: current difficulty.
 * Outputs: mini-game result.
 */
MiniGameResult MiniGameManager::playRandomMiniGame(Difficulty difficulty) {
    std::uniform_int_distribution<int> gameDistribution(0, 3);
    int selected = gameDistribution(randomEngine_);

    if (selected == 0) {
        return playNumberGuess(difficulty);
    }

    if (selected == 1) {
        return playMathQuiz(difficulty);
    }

    if (selected == 2) {
        return playMemoryTrial(difficulty);
    }

    return playRockPaperScissors(difficulty);
}
