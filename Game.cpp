#include "Game.h"

#include "Item.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <limits>
#include <queue>
#include <sstream>
#include <sys/select.h>
#include <unistd.h>

/**
 * Returns the ANSI reset sequence.
 * Inputs: none.
 * Outputs: ANSI reset text.
 */
static std::string colorReset() {
    return "\033[0m";
}

/**
 * Wraps text in an ANSI color code.
 * Inputs: ANSI color code and plain text.
 * Outputs: colored text string.
 */
static std::string colorText(const std::string& colorCode, const std::string& text) {
    return colorCode + text + colorReset();
}

/**
 * Builds a simple colored resource bar.
 * Inputs: current value, maximum value, ANSI color code, fill character, and bar width.
 * Outputs: formatted colored bar string.
 */
static std::string makeBar(int current, int maximum, const std::string& colorCode, char fillCharacter, int width) {
    if (maximum <= 0) {
        maximum = 1;
    }

    if (current < 0) {
        current = 0;
    }

    if (current > maximum) {
        current = maximum;
    }

    int filled = (current * width) / maximum;
    std::ostringstream output;
    output << "[";

    for (int index = 0; index < width; ++index) {
        if (index < filled) {
            output << colorCode << fillCharacter << colorReset();
        } else {
            output << '-';
        }
    }

    output << "]";
    return output.str();
}

/**
 * Returns a colored urgency label for remaining time.
 * Inputs: remaining seconds in Time Trial mode.
 * Outputs: colored urgency text.
 */
static std::string timePressureLabel(int seconds) {
    if (seconds <= 25) {
        return colorText("\033[1;97;41m", "CRITICAL");
    }

    if (seconds <= 60) {
        return colorText("\033[1;91m", "DANGER");
    }

    if (seconds <= 120) {
        return colorText("\033[1;93m", "LOW");
    }

    return colorText("\033[1;92m", "STABLE");
}

/**
 * Decorates event log text using colors by message category.
 * Inputs: plain log message.
 * Outputs: colored log line string.
 */
static std::string decorateLogLine(const std::string& message) {
    if (message.find("sentinel") != std::string::npos || message.find("caught") != std::string::npos) {
        return colorText("\033[1;91m", message);
    }

    if (message.find("Time") != std::string::npos || message.find("clock") != std::string::npos) {
        return colorText("\033[1;93m", message);
    }

    if (message.find("saved") != std::string::npos || message.find("loaded") != std::string::npos) {
        return colorText("\033[1;94m", message);
    }

    if (message.find("Key") != std::string::npos || message.find("Victory") != std::string::npos) {
        return colorText("\033[1;92m", message);
    }

    return message;
}

/**
 * Waits up to timeoutMs for one full input line.
 * Inputs: output line reference and timeout in milliseconds.
 * Outputs: true when a line is read, false on timeout or stream end.
 */
static bool readLineWithTimeout(std::string& line, int timeoutMs) {
    fd_set inputSet;
    FD_ZERO(&inputSet);
    FD_SET(STDIN_FILENO, &inputSet);

    struct timeval waitTime;
    waitTime.tv_sec = timeoutMs / 1000;
    waitTime.tv_usec = (timeoutMs % 1000) * 1000;

    int ready = select(STDIN_FILENO + 1, &inputSet, NULL, NULL, &waitTime);
    if (ready <= 0) {
        return false;
    }

    return static_cast<bool>(std::getline(std::cin, line));
}

/**
 * Creates a game controller and seeds its random generator.
 * Inputs: executable directory string.
 * Outputs: initialized game object.
 */
Game::Game(const std::string& baseDirectory)
    : player_(NULL),
      map_(NULL),
      eventManager_(NULL),
      miniGameManager_(NULL),
      switchActive_(false),
      runningGame_(false),
      playerWon_(false),
      timeExpired_(false),
      sentinelCaught_(false),
      currentTimeline_(Timeline::PAST),
      difficulty_(Difficulty::EASY),
      mode_(GameMode::CASUAL),
      moves_(0),
      timeLimitSeconds_(0),
      baseDirectory_(baseDirectory.empty() ? "." : baseDirectory),
      pastSentinel_{false, {0, 0}, false, 0},
      presentSentinel_{false, {0, 0}, false, 0},
      timerStart_(std::chrono::steady_clock::now()),
      pastSentinelLastMove_(std::chrono::steady_clock::now()),
      presentSentinelLastMove_(std::chrono::steady_clock::now()),
      randomEngine_(static_cast<unsigned int>(std::time(NULL))) {
}

/**
 * Frees all dynamically allocated resources.
 * Inputs: none.
 * Outputs: none.
 */
Game::~Game() {
    clearResources();
}

/**
 * Deletes dynamically allocated game objects and resets pointers.
 * Inputs: none.
 * Outputs: none.
 */
void Game::clearResources() {
    delete player_;
    delete map_;
    delete eventManager_;
    delete miniGameManager_;

    player_ = NULL;
    map_ = NULL;
    eventManager_ = NULL;
    miniGameManager_ = NULL;
}

/**
 * Runs the full menu and gameplay program.
 * Inputs: none.
 * Outputs: none.
 */
void Game::run() {
    bool programRunning = true;

    while (programRunning) {
        clearScreen();
        showMainMenu();

        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1") {
            GameMode selectedMode = GameMode::CASUAL;
            Difficulty selectedDifficulty = Difficulty::EASY;

            if (!promptForGameMode(selectedMode)) {
                std::cout << "Invalid mode selection.\n";
                waitForEnter();
                continue;
            }

            if (!promptForDifficulty(selectedDifficulty)) {
                std::cout << "Invalid difficulty selection.\n";
                waitForEnter();
                continue;
            }

            if (startNewGame(selectedDifficulty, selectedMode)) {
                gameLoop();
            } else {
                waitForEnter();
            }
        } else if (choice == "2") {
            if (loadSavedGame()) {
                gameLoop();
            } else {
                waitForEnter();
            }
        } else if (choice == "3") {
            clearScreen();
            showHelp();
            waitForEnter();
        } else if (choice == "4") {
            programRunning = false;
        } else {
            std::cout << "Invalid menu option.\n";
            waitForEnter();
        }
    }
}

/**
 * Prints the main menu.
 * Inputs: none.
 * Outputs: terminal menu text.
 */
void Game::showMainMenu() const {
    std::cout << "====================================\n";
    std::cout << "        ECHOES OF TIME\n";
    std::cout << "====================================\n";
    std::cout << "1. New Game\n";
    std::cout << "2. Load Game\n";
    std::cout << "3. Help\n";
    std::cout << "4. Exit\n";
    std::cout << "\nChoose an option: ";
}

/**
 * Prints the in-game help screen.
 * Inputs: none.
 * Outputs: terminal help text.
 */
void Game::showHelp() const {
    std::cout << "Goal:\n";
    std::cout << "Escape the ruin by using the Past and Present together.\n\n";
    std::cout << "Game Modes:\n";
    std::cout << "1. Casual Mode: the original puzzle adventure with random events.\n";
    std::cout << "2. Time Trial: random-event tiles launch one of four mini-games, and every result costs time.\n";
    std::cout << "3. Sentinel Chase: each timeline has its own sentinel using LOS + close-range detection and path chase.\n\n";
    std::cout << "Commands:\n";
    std::cout << "W/A/S/D - Move\n";
    std::cout << "T - Switch timeline\n";
    std::cout << "I - Show inventory\n";
    std::cout << "V - Save game\n";
    std::cout << "L - Load game\n";
    std::cout << "H - Show help\n";
    std::cout << "Q - Quit to main menu\n\n";
    std::cout << "Symbols:\n";
    std::cout << "# wall, . floor, P player, K key, D locked door, S switch\n";
    std::cout << "? random event or mini-game, ^ trap, E hazard, G sentinel, X final exit\n";
}

/**
 * Prompts the player to choose a difficulty.
 * Inputs: output difficulty reference.
 * Outputs: true when a valid choice is made.
 */
bool Game::promptForDifficulty(Difficulty& difficulty) const {
    clearScreen();
    std::cout << "Select difficulty:\n";
    std::cout << "1. Easy\n";
    std::cout << "2. Normal\n";
    std::cout << "3. Hard\n";
    std::cout << "> ";

    std::string choice;
    std::getline(std::cin, choice);

    if (choice == "1") {
        difficulty = Difficulty::EASY;
        return true;
    }

    if (choice == "2") {
        difficulty = Difficulty::NORMAL;
        return true;
    }

    if (choice == "3") {
        difficulty = Difficulty::HARD;
        return true;
    }

    return false;
}

/**
 * Prompts the player to choose a game mode.
 * Inputs: output game mode reference.
 * Outputs: true when a valid choice is made.
 */
bool Game::promptForGameMode(GameMode& mode) const {
    clearScreen();
    std::cout << "Select game mode:\n";
    std::cout << "1. Casual Mode\n";
    std::cout << "2. Time Trial Mode\n";
    std::cout << "3. Sentinel Chase Mode\n";
    std::cout << "> ";

    std::string choice;
    std::getline(std::cin, choice);

    if (choice == "1") {
        mode = GameMode::CASUAL;
        return true;
    }

    if (choice == "2") {
        mode = GameMode::TIME_TRIAL;
        return true;
    }

    if (choice == "3") {
        mode = GameMode::SENTINEL_CHASE;
        return true;
    }

    return false;
}

/**
 * Starts a new game using the selected difficulty and mode.
 * Inputs: difficulty value and game mode.
 * Outputs: true when setup succeeds.
 */
bool Game::startNewGame(Difficulty difficulty, GameMode mode) {
    clearResources();

    difficulty_ = difficulty;
    mode_ = mode;
    switchActive_ = false;
    runningGame_ = true;
    playerWon_ = false;
    currentTimeline_ = Timeline::PAST;
    moves_ = 0;
    eventLog_.clear();

    DifficultySettings settings = getSettings(difficulty);
    player_ = new Player(settings.startHp, settings.startEnergy);
    map_ = new Map();
    eventManager_ = new EventManager();
    miniGameManager_ = new MiniGameManager();
    eventManager_->configure(difficulty);

    std::string pastPath;
    std::string presentPath;
    std::string errorMessage;
    getMapPaths(difficulty, pastPath, presentPath);

    if (!map_->loadFromFiles(pastPath, presentPath, errorMessage)) {
        std::cout << errorMessage << "\n";
        runningGame_ = false;
        clearResources();
        return false;
    }

    applyRunVariation();
    player_->setPosition(map_->getStartPosition());
    initializeModeState();
    addLog("You awaken in the fractured ruin. Find the Time Key and escape.");
    return true;
}

/**
 * Loads game state from the default save file.
 * Inputs: none.
 * Outputs: true when loading succeeds.
 */
bool Game::loadSavedGame() {
    SaveData data;
    std::string errorMessage;

    if (!SaveSystem::loadGame(resolveProjectPath("data/save.txt"), data, errorMessage)) {
        std::cout << errorMessage << "\n";
        return false;
    }

    if (!applySaveData(data, errorMessage)) {
        std::cout << errorMessage << "\n";
        return false;
    }

    addLog("Saved game loaded.");
    return true;
}

/**
 * Restores runtime objects from loaded save data.
 * Inputs: save data and error message reference.
 * Outputs: true when restoration succeeds.
 */
bool Game::applySaveData(const SaveData& data, std::string& errorMessage) {
    clearResources();

    difficulty_ = data.difficulty;
    mode_ = data.mode;
    currentTimeline_ = data.timeline;
    switchActive_ = data.switchActive;
    moves_ = data.moves;
    runningGame_ = true;
    playerWon_ = false;
    timeExpired_ = false;
    sentinelCaught_ = false;
    eventLog_.clear();

    player_ = new Player(data.maxHp, data.maxEnergy);
    player_->setStats(data.hp, data.maxHp, data.energy, data.maxEnergy);
    player_->setPosition(data.position);
    player_->restoreInventory(data.inventoryCounts);

    map_ = new Map();
    if (!map_->setFromSavedGrids(data.pastGrid, data.presentGrid, errorMessage)) {
        runningGame_ = false;
        clearResources();
        return false;
    }

    eventManager_ = new EventManager();
    miniGameManager_ = new MiniGameManager();
    eventManager_->configure(difficulty_);

    if (!map_->inBounds(data.position.row, data.position.col)) {
        errorMessage = "Saved player position is outside the map.";
        runningGame_ = false;
        clearResources();
        return false;
    }

    timeLimitSeconds_ = data.remainingTimeSeconds;
    timerStart_ = std::chrono::steady_clock::now();
    pastSentinelLastMove_ = std::chrono::steady_clock::now();
    presentSentinelLastMove_ = std::chrono::steady_clock::now();
    pastSentinel_.exists = data.pastSentinel.exists;
    pastSentinel_.position = data.pastSentinel.position;
    pastSentinel_.alerted = data.pastSentinel.alerted;
    pastSentinel_.stepCounter = data.pastSentinel.stepCounter;
    presentSentinel_.exists = data.presentSentinel.exists;
    presentSentinel_.position = data.presentSentinel.position;
    presentSentinel_.alerted = data.presentSentinel.alerted;
    presentSentinel_.stepCounter = data.presentSentinel.stepCounter;

    return true;
}

/**
 * Runs the active gameplay loop.
 * Inputs: none.
 * Outputs: none.
 */
void Game::gameLoop() {
    while (runningGame_ && player_ != NULL && map_ != NULL) {
        if (mode_ == GameMode::TIME_TRIAL && getRemainingTimeSeconds() <= 0) {
            timeExpired_ = true;
            runningGame_ = false;
            break;
        }

        renderGame();

        std::string commandLine;
        if (mode_ == GameMode::SENTINEL_CHASE) {
            bool lineReady = false;

            while (runningGame_ && !lineReady) {
                if (!std::cin.good()) {
                    runningGame_ = false;
                    break;
                }

                if (readLineWithTimeout(commandLine, 120)) {
                    lineReady = true;
                    break;
                }

                std::size_t previousLogSize = eventLog_.size();
                SentinelState previousSentinel = sentinelForTimeline(currentTimeline_);

                updateSentinelChase();

                if (!runningGame_) {
                    break;
                }

                const SentinelState& currentSentinel = sentinelForTimeline(currentTimeline_);
                bool sentinelChanged = previousSentinel.alerted != currentSentinel.alerted
                    || !samePosition(previousSentinel.position, currentSentinel.position);
                bool logChanged = eventLog_.size() != previousLogSize;

                if (sentinelChanged || logChanged) {
                    renderGame();
                }
            }

            if (!runningGame_) {
                break;
            }

            if (!lineReady) {
                if (!std::cin.good()) {
                    runningGame_ = false;
                    break;
                }
                continue;
            }
        } else if (!std::getline(std::cin, commandLine)) {
            runningGame_ = false;
            break;
        }

        if (mode_ == GameMode::TIME_TRIAL && getRemainingTimeSeconds() <= 0) {
            timeExpired_ = true;
            runningGame_ = false;
            break;
        }

        if (commandLine.empty()) {
            addLog("Enter a command such as W, A, S, D, or T.");
            continue;
        }

        char command = static_cast<char>(std::toupper(static_cast<unsigned char>(commandLine[0])));
        processCommand(command);

        if (player_ != NULL && !player_->isAlive()) {
            runningGame_ = false;
        }
    }

    if (player_ == NULL) {
        return;
    }

    clearScreen();

    if (playerWon_) {
        std::cout << "You place the Time Key into the final seal and escape the ruin.\n";
        if (mode_ == GameMode::TIME_TRIAL) {
            std::cout << "Victory with " << getRemainingTimeSeconds() << " seconds remaining.\n";
        } else {
            std::cout << "Victory. Total moves: " << moves_ << "\n";
        }
    } else if (timeExpired_) {
        std::cout << "Time runs out before you can escape.\n";
        std::cout << "Game Over.\n";
    } else if (sentinelCaught_) {
        std::cout << "A sentinel catches you inside the ruin.\n";
        std::cout << "Game Over.\n";
    } else if (!player_->isAlive()) {
        std::cout << "The ruin claims another victim. Your HP fell to zero.\n";
        std::cout << "Game Over.\n";
    } else {
        std::cout << "You leave the current run and return to the main menu.\n";
    }

    waitForEnter();
}

/**
 * Prints the current game state and map.
 * Inputs: none.
 * Outputs: terminal HUD and map.
 */
void Game::renderGame() const {
    clearScreen();

    DifficultySettings settings = getSettings(difficulty_);
    std::string timelineLabel = currentTimeline_ == Timeline::PAST
        ? colorText("\033[1;95m", "PAST")
        : colorText("\033[1;96m", "PRESENT");
    std::string modeLabel;

    if (mode_ == GameMode::CASUAL) {
        modeLabel = colorText("\033[1;92m", "CASUAL");
    } else if (mode_ == GameMode::TIME_TRIAL) {
        modeLabel = colorText("\033[1;93m", "TIME TRIAL");
    } else {
        modeLabel = colorText("\033[1;91m", "SENTINEL CHASE");
    }

    int hpPercent = (player_->getHp() * 100) / std::max(1, player_->getMaxHp());
    int energyPercent = (player_->getEnergy() * 100) / std::max(1, player_->getMaxEnergy());
    std::string hpColor = hpPercent <= 30 ? "\033[1;97;41m" : (hpPercent <= 60 ? "\033[1;93m" : "\033[1;92m");
    std::string energyColor = energyPercent <= 25 ? "\033[1;91m" : (energyPercent <= 55 ? "\033[1;93m" : "\033[1;96m");
    std::string hpBar = makeBar(player_->getHp(), player_->getMaxHp(), hpColor, '#', 14);
    std::string energyBar = makeBar(player_->getEnergy(), player_->getMaxEnergy(), energyColor, '*', 14);

    std::cout << "============================================================\n";
    std::cout << colorText("\033[1;97;44m", " ECHOES OF TIME ") << "\n";
    std::cout << "Mode: " << modeLabel << " | Difficulty: " << difficultyToString(difficulty_);
    std::cout << " | Timeline: " << timelineLabel;
    std::cout << " | Moves: " << moves_ << "\n";
    std::cout << "HP     : " << hpBar << " " << player_->getHp() << "/" << player_->getMaxHp();
    std::cout << " (" << hpPercent << "%)\n";
    std::cout << "Energy : " << energyBar << " " << player_->getEnergy() << "/" << player_->getMaxEnergy();
    std::cout << " (" << energyPercent << "%)";
    std::cout << " | Switch Cost: " << settings.switchCost << "\n";
    std::cout << "Inventory: " << player_->inventorySummary() << "\n";

    if (mode_ == GameMode::TIME_TRIAL) {
        int secondsLeft = getRemainingTimeSeconds();
        std::cout << "Time Left: " << secondsLeft << "s  (" << timePressureLabel(secondsLeft) << ")\n";
    }

    if (mode_ == GameMode::SENTINEL_CHASE) {
        const SentinelState& currentSentinel = sentinelForTimeline(currentTimeline_);
        std::cout << "Sentinel: "
                  << (currentSentinel.alerted
                          ? colorText("\033[1;91m", "ALERTED")
                          : colorText("\033[1;90m", "DORMANT"))
                  << " | Detect: same-row/col LOS or close-square scan\n";
    }

    if (settings.showHints) {
        std::cout << "Hint: " << objectiveText() << "\n";
    }

    std::cout << "============================================================\n";
    map_->render(currentTimeline_, player_->getPosition(), player_->hasItem("TimeKey"), switchActive_, currentSentinelPosition());
    std::cout << "Legend: "
              << colorText("\033[1;36m", "P") << " player  "
              << colorText("\033[1;93m", "K") << " key  "
              << colorText("\033[1;95m", "S") << " switch  "
              << colorText("\033[1;33m", "D") << " locked door  "
              << colorText("\033[1;92m", "/") << " open door  "
              << colorText("\033[1;94m", "?") << " event/mini-game  "
              << colorText("\033[1;91m", "^/E") << " danger  ";

    if (mode_ == GameMode::SENTINEL_CHASE) {
        std::cout << colorText("\033[1;97;41m", "G") << " sentinel  ";
    }

    std::cout << colorText("\033[1;92m", "X") << " exit\n";
    std::cout << "------------------------------------\n";
    std::cout << "Recent events:\n";

    if (eventLog_.empty()) {
        std::cout << "- " << colorText("\033[0;37m", "No recent events.") << "\n";
    } else {
        for (std::size_t index = 0; index < eventLog_.size(); ++index) {
            std::cout << "- " << decorateLogLine(eventLog_[index]) << "\n";
        }
    }

    std::cout << "------------------------------------\n";
    std::cout << "Commands: W/A/S/D move, T switch, I inventory, V save, L load, H help, Q quit\n";
    std::cout << "> ";
    std::cout.flush();
}

/**
 * Processes one player command.
 * Inputs: command character.
 * Outputs: none.
 */
void Game::processCommand(char command) {
    bool consumedTurn = false;

    if (command == 'W') {
        consumedTurn = attemptMove(-1, 0);
    } else if (command == 'A') {
        consumedTurn = attemptMove(0, -1);
    } else if (command == 'S') {
        consumedTurn = attemptMove(1, 0);
    } else if (command == 'D') {
        consumedTurn = attemptMove(0, 1);
    } else if (command == 'T') {
        consumedTurn = attemptTimelineSwitch();
    } else if (command == 'I') {
        addLog("Inventory: " + player_->inventorySummary());
    } else if (command == 'V') {
        saveCurrentGame();
    } else if (command == 'L') {
        loadSavedGame();
    } else if (command == 'H') {
        clearScreen();
        showHelp();
        waitForEnter();
    } else if (command == 'Q') {
        runningGame_ = false;
    } else {
        addLog("Unknown command. Press H for help.");
    }

    updateModeAfterAction(consumedTurn);
}

/**
 * Attempts player movement by one tile.
 * Inputs: row delta and column delta.
 * Outputs: true when the move succeeds.
 */
bool Game::attemptMove(int rowDelta, int colDelta) {
    Position currentPosition = player_->getPosition();
    int nextRow = currentPosition.row + rowDelta;
    int nextCol = currentPosition.col + colDelta;

    if (!map_->isWalkable(currentTimeline_, nextRow, nextCol, player_->hasItem("TimeKey"), switchActive_)) {
        addLog(map_->getBlockReason(currentTimeline_, nextRow, nextCol, player_->hasItem("TimeKey"), switchActive_));
        return false;
    }

    Position nextPosition;
    nextPosition.row = nextRow;
    nextPosition.col = nextCol;
    player_->setPosition(nextPosition);
    moves_++;

    char tile = map_->getTile(currentTimeline_, nextRow, nextCol);
    handleTile(tile);
    return true;
}

/**
 * Attempts to switch between Past and Present.
 * Inputs: none.
 * Outputs: true when the switch succeeds.
 */
bool Game::attemptTimelineSwitch() {
    DifficultySettings settings = getSettings(difficulty_);

    if (player_->getEnergy() < settings.switchCost) {
        addLog("Not enough energy to switch timelines.");
        return false;
    }

    Timeline targetTimeline = currentTimeline_ == Timeline::PAST ? Timeline::PRESENT : Timeline::PAST;
    Position position = player_->getPosition();

    if (!map_->isWalkable(targetTimeline, position.row, position.col, player_->hasItem("TimeKey"), switchActive_)) {
        addLog("The other timeline rejects the switch at this position.");
        return false;
    }

    player_->changeEnergy(-settings.switchCost);
    currentTimeline_ = targetTimeline;

    if (currentTimeline_ == Timeline::PAST) {
        addLog("You pull the ruin backward into the Past.");
    } else {
        addLog("You force the ruin forward into the Present.");
    }

    char tile = map_->getTile(currentTimeline_, position.row, position.col);
    handleTile(tile);
    return true;
}

/**
 * Applies the effect of the tile currently under the player.
 * Inputs: tile character.
 * Outputs: none.
 */
void Game::handleTile(char tile) {
    Position position = player_->getPosition();
    DifficultySettings settings = getSettings(difficulty_);

    if (tile == 'K' && !player_->hasItem("TimeKey")) {
        player_->addItem(createItemById("TimeKey"));
        map_->setTileBoth(position.row, position.col, '.');
        addLog("You recovered the Time Key. The exit can now be unlocked.");
    } else if (tile == 'S') {
        if (!switchActive_) {
            switchActive_ = true;
            addLog("You activate the ancient switch. Doors marked D are now open.");
        } else {
            addLog("The switch is already active.");
        }
    } else if (tile == '?') {
        map_->setTile(currentTimeline_, position.row, position.col, '.');

        if (mode_ == GameMode::TIME_TRIAL) {
            MiniGameResult result = miniGameManager_->playRandomMiniGame(difficulty_);
            addLog("Mini-game - " + result.name + ": " + result.summary);
            applyTimePenalty(result.timePenaltySeconds, result.won ? "You cleared the challenge, but the clock still advances." : "You fail the challenge and lose precious time.");
        } else {
            addLog(eventManager_->triggerRandomEvent(*player_));
        }
    } else if (tile == '^') {
        int damage = randomInt(settings.trapMinDamage, settings.trapMaxDamage);
        player_->changeHp(-damage);
        addLog("A trap pierces the floor beneath you. HP -" + std::to_string(damage) + ".");
    } else if (tile == 'E') {
        int damage = randomInt(settings.trapMinDamage + 1, settings.trapMaxDamage + 2);
        player_->changeHp(-damage);
        map_->setTile(currentTimeline_, position.row, position.col, '.');
        addLog("A temporal shade attacks and fades away. HP -" + std::to_string(damage) + ".");
    } else if (tile == 'X') {
        playerWon_ = true;
        runningGame_ = false;
    }
}

/**
 * Adds a line to the recent event log.
 * Inputs: log message.
 * Outputs: none.
 */
void Game::addLog(const std::string& message) {
    eventLog_.push_back(message);

    if (eventLog_.size() > 7) {
        eventLog_.erase(eventLog_.begin());
    }
}

/**
 * Saves the current game to disk.
 * Inputs: none.
 * Outputs: none.
 */
void Game::saveCurrentGame() {
    SaveData data;
    data.difficulty = difficulty_;
    data.mode = mode_;
    data.timeline = currentTimeline_;
    data.position = player_->getPosition();
    data.hp = player_->getHp();
    data.maxHp = player_->getMaxHp();
    data.energy = player_->getEnergy();
    data.maxEnergy = player_->getMaxEnergy();
    data.switchActive = switchActive_;
    data.moves = moves_;
    data.remainingTimeSeconds = mode_ == GameMode::TIME_TRIAL ? getRemainingTimeSeconds() : 0;
    data.pastSentinel.exists = pastSentinel_.exists;
    data.pastSentinel.position = pastSentinel_.position;
    data.pastSentinel.alerted = pastSentinel_.alerted;
    data.pastSentinel.stepCounter = pastSentinel_.stepCounter;
    data.presentSentinel.exists = presentSentinel_.exists;
    data.presentSentinel.position = presentSentinel_.position;
    data.presentSentinel.alerted = presentSentinel_.alerted;
    data.presentSentinel.stepCounter = presentSentinel_.stepCounter;
    data.inventoryCounts = player_->getItemCounts();
    data.pastGrid = map_->getPastGrid();
    data.presentGrid = map_->getPresentGrid();

    std::string savePath = resolveProjectPath("data/save.txt");
    std::string errorMessage;
    if (SaveSystem::saveGame(savePath, data, errorMessage)) {
        addLog("Game saved to " + savePath + ".");
    } else {
        addLog(errorMessage);
    }
}

/**
 * Converts the difficulty into gameplay settings.
 * Inputs: difficulty value.
 * Outputs: settings structure.
 */
Game::DifficultySettings Game::getSettings(Difficulty difficulty) const {
    DifficultySettings settings;

    if (difficulty == Difficulty::EASY) {
        settings.startHp = 14;
        settings.startEnergy = 8;
        settings.switchCost = 1;
        settings.trapMinDamage = 1;
        settings.trapMaxDamage = 2;
        settings.showHints = true;
        settings.timeLimitSeconds = 300;
        settings.sentinelCloseSquareRadius = 3;
        settings.sentinelMovePeriodMs = 1400;
        settings.sentinelMovesPerTick = 1;
    } else if (difficulty == Difficulty::NORMAL) {
        settings.startHp = 10;
        settings.startEnergy = 6;
        settings.switchCost = 2;
        settings.trapMinDamage = 2;
        settings.trapMaxDamage = 3;
        settings.showHints = true;
        settings.timeLimitSeconds = 240;
        settings.sentinelCloseSquareRadius = 3;
        settings.sentinelMovePeriodMs = 1000;
        settings.sentinelMovesPerTick = 1;
    } else {
        settings.startHp = 8;
        settings.startEnergy = 4;
        settings.switchCost = 3;
        settings.trapMinDamage = 3;
        settings.trapMaxDamage = 4;
        settings.showHints = false;
        settings.timeLimitSeconds = 180;
        settings.sentinelCloseSquareRadius = 4;
        settings.sentinelMovePeriodMs = 750;
        settings.sentinelMovesPerTick = 1;
    }

    return settings;
}

/**
 * Returns the map file paths for the current difficulty.
 * Inputs: difficulty value and output path references.
 * Outputs: none.
 */
void Game::getMapPaths(Difficulty difficulty, std::string& pastPath, std::string& presentPath) const {
    if (difficulty == Difficulty::EASY) {
        pastPath = resolveProjectPath("maps/easy_past.txt");
        presentPath = resolveProjectPath("maps/easy_present.txt");
    } else if (difficulty == Difficulty::NORMAL) {
        pastPath = resolveProjectPath("maps/normal_past.txt");
        presentPath = resolveProjectPath("maps/normal_present.txt");
    } else {
        pastPath = resolveProjectPath("maps/hard_past.txt");
        presentPath = resolveProjectPath("maps/hard_present.txt");
    }
}

/**
 * Resolves a project file path from either the current directory or the executable directory.
 * Inputs: relative project path such as a map or save file.
 * Outputs: usable file path string.
 */
std::string Game::resolveProjectPath(const std::string& relativePath) const {
    namespace fs = std::filesystem;

    fs::path directPath(relativePath);
    if (fs::exists(directPath)) {
        return directPath.string();
    }

    fs::path resolvedPath = fs::path(baseDirectory_) / relativePath;
    return resolvedPath.string();
}

/**
 * Draws spacing to simulate a screen refresh.
 * Inputs: none.
 * Outputs: terminal blank lines.
 */
void Game::clearScreen() const {
    std::cout << "\033[2J\033[H";
}

/**
 * Waits for the user to press Enter.
 * Inputs: none.
 * Outputs: none.
 */
void Game::waitForEnter() const {
    std::cout << "\nPress Enter to continue...";
    std::string line;
    std::getline(std::cin, line);
}

/**
 * Generates a random integer inside a closed range.
 * Inputs: minimum and maximum value.
 * Outputs: pseudo-random integer.
 */
int Game::randomInt(int minimum, int maximum) {
    std::uniform_int_distribution<int> distribution(minimum, maximum);
    return distribution(randomEngine_);
}

/**
 * Returns the current objective or hint text.
 * Inputs: none.
 * Outputs: short status string.
 */
std::string Game::objectiveText() const {
    if (mode_ == GameMode::TIME_TRIAL) {
        if (!player_->hasItem("TimeKey")) {
            return "Find the Time Key quickly. Random event tiles now launch mini-games.";
        }

        if (!switchActive_) {
            return "Open the sealed door before the clock runs out.";
        }

        return "Reach the exit before time reaches zero.";
    }

    if (mode_ == GameMode::SENTINEL_CHASE) {
        if (!player_->hasItem("TimeKey")) {
            return "Find the Time Key. Sentinels detect by clear line-of-sight or close-range scan.";
        }

        if (!switchActive_) {
            return "Activate the switch and break line-of-sight before sentinels close in.";
        }

        return "Reach the exit while the active timeline sentinel hunts your position.";
    }

    if (!player_->hasItem("TimeKey")) {
        return "Search the Past for the Time Key.";
    }

    if (!switchActive_) {
        return "Use either timeline to find the switch and open the sealed door.";
    }

    if (currentTimeline_ == Timeline::PAST) {
        return "The way out lies in the Present. Shift when the tile is clear.";
    }

    return "Reach X to escape.";
}

/**
 * Applies lightweight random map variation for each new run.
 * Inputs: none.
 * Outputs: none.
 */
void Game::applyRunVariation() {
    if (map_ == NULL) {
        return;
    }

    Position start = map_->getStartPosition();
    bool changedAny = false;
    const Timeline timelines[2] = {Timeline::PAST, Timeline::PRESENT};

    for (int timelineIndex = 0; timelineIndex < 2; ++timelineIndex) {
        Timeline timeline = timelines[timelineIndex];
        const std::vector<std::string>& gridRef = timeline == Timeline::PAST ? map_->getPastGrid() : map_->getPresentGrid();

        std::vector<Position> sourcePositions;
        std::vector<char> sourceTiles;
        std::vector<Position> candidateFloorTiles;

        for (std::size_t row = 0; row < gridRef.size(); ++row) {
            for (std::size_t col = 0; col < gridRef[row].size(); ++col) {
                char tile = map_->getTile(timeline, static_cast<int>(row), static_cast<int>(col));

                if (tile == '?' || tile == '^' || tile == 'E') {
                    sourcePositions.push_back(Position{static_cast<int>(row), static_cast<int>(col)});
                    sourceTiles.push_back(tile);
                    continue;
                }

                if (tile == '.') {
                    int distanceFromStart = std::abs(static_cast<int>(row) - start.row)
                        + std::abs(static_cast<int>(col) - start.col);
                    if (distanceFromStart >= 2) {
                        candidateFloorTiles.push_back(Position{static_cast<int>(row), static_cast<int>(col)});
                    }
                }
            }
        }

        if (sourceTiles.empty() || candidateFloorTiles.empty()) {
            continue;
        }

        for (std::size_t index = 0; index < sourcePositions.size(); ++index) {
            map_->setTile(timeline, sourcePositions[index].row, sourcePositions[index].col, '.');
        }

        std::shuffle(candidateFloorTiles.begin(), candidateFloorTiles.end(), randomEngine_);
        std::shuffle(sourceTiles.begin(), sourceTiles.end(), randomEngine_);
        std::size_t placeCount = std::min(candidateFloorTiles.size(), sourceTiles.size());

        for (std::size_t index = 0; index < placeCount; ++index) {
            map_->setTile(timeline, candidateFloorTiles[index].row, candidateFloorTiles[index].col, sourceTiles[index]);
            changedAny = true;
        }
    }

    if (changedAny) {
        addLog("The ruin shifts. Hazards and anomalies appear in new places.");
    }
}

/**
 * Initializes mode-specific runtime state for a new game.
 * Inputs: none.
 * Outputs: none.
 */
void Game::initializeModeState() {
    timeExpired_ = false;
    sentinelCaught_ = false;
    timeLimitSeconds_ = getSettings(difficulty_).timeLimitSeconds;
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    timerStart_ = now;
    pastSentinelLastMove_ = now;
    presentSentinelLastMove_ = now;
    pastSentinel_ = SentinelState{false, {0, 0}, false, 0};
    presentSentinel_ = SentinelState{false, {0, 0}, false, 0};

    if (mode_ == GameMode::SENTINEL_CHASE) {
        initializeSentinels();
    }
}

/**
 * Returns the remaining time in Time Trial mode.
 * Inputs: none.
 * Outputs: remaining seconds, clamped at zero.
 */
int Game::getRemainingTimeSeconds() const {
    if (mode_ != GameMode::TIME_TRIAL) {
        return 0;
    }

    int elapsed = static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - timerStart_).count());
    int remaining = timeLimitSeconds_ - elapsed;
    return remaining > 0 ? remaining : 0;
}

/**
 * Applies a time penalty in Time Trial mode.
 * Inputs: penalty amount in seconds and a log reason.
 * Outputs: none.
 */
void Game::applyTimePenalty(int seconds, const std::string& reason) {
    if (mode_ != GameMode::TIME_TRIAL) {
        return;
    }

    timeLimitSeconds_ -= seconds;
    if (timeLimitSeconds_ < 0) {
        timeLimitSeconds_ = 0;
    }

    addLog(reason + " Time -" + std::to_string(seconds) + "s.");

    if (getRemainingTimeSeconds() <= 0) {
        timeExpired_ = true;
        runningGame_ = false;
    }
}

/**
 * Returns the mutable sentinel state for a timeline.
 * Inputs: timeline value.
 * Outputs: sentinel state reference.
 */
Game::SentinelState& Game::sentinelForTimeline(Timeline timeline) {
    return timeline == Timeline::PAST ? pastSentinel_ : presentSentinel_;
}

/**
 * Returns the read-only sentinel state for a timeline.
 * Inputs: timeline value.
 * Outputs: sentinel state reference.
 */
const Game::SentinelState& Game::sentinelForTimeline(Timeline timeline) const {
    return timeline == Timeline::PAST ? pastSentinel_ : presentSentinel_;
}

/**
 * Returns the mutable sentinel movement clock for a timeline.
 * Inputs: timeline value.
 * Outputs: last-move time point reference.
 */
std::chrono::steady_clock::time_point& Game::sentinelClockForTimeline(Timeline timeline) {
    return timeline == Timeline::PAST ? pastSentinelLastMove_ : presentSentinelLastMove_;
}

/**
 * Returns the read-only sentinel movement clock for a timeline.
 * Inputs: timeline value.
 * Outputs: last-move time point reference.
 */
const std::chrono::steady_clock::time_point& Game::sentinelClockForTimeline(Timeline timeline) const {
    return timeline == Timeline::PAST ? pastSentinelLastMove_ : presentSentinelLastMove_;
}

/**
 * Randomly places one sentinel in each timeline.
 * Inputs: none.
 * Outputs: none.
 */
void Game::initializeSentinels() {
    Position playerPosition = player_->getPosition();
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    pastSentinel_.exists = true;
    pastSentinel_.position = chooseSentinelSpawn(Timeline::PAST, playerPosition);
    pastSentinel_.alerted = false;
    pastSentinel_.stepCounter = 0;
    pastSentinelLastMove_ = now;

    presentSentinel_.exists = true;
    presentSentinel_.position = chooseSentinelSpawn(Timeline::PRESENT, playerPosition);
    presentSentinel_.alerted = false;
    presentSentinel_.stepCounter = 0;
    presentSentinelLastMove_ = now;
}

/**
 * Chooses a valid random sentinel spawn position on a floor tile.
 * Inputs: timeline and player position to avoid.
 * Outputs: chosen sentinel position.
 */
Position Game::chooseSentinelSpawn(Timeline timeline, const Position& avoidPosition) {
    std::vector<Position> safeCandidates;
    std::vector<Position> allCandidates;
    const std::vector<std::string>& grid = timeline == Timeline::PAST ? map_->getPastGrid() : map_->getPresentGrid();

    for (std::size_t row = 0; row < grid.size(); ++row) {
        for (std::size_t col = 0; col < grid[row].size(); ++col) {
            if (grid[row][col] == '.') {
                Position candidate = {static_cast<int>(row), static_cast<int>(col)};
                allCandidates.push_back(candidate);
                int distance = std::abs(candidate.row - avoidPosition.row) + std::abs(candidate.col - avoidPosition.col);
                if (distance >= 4) {
                    safeCandidates.push_back(candidate);
                }
            }
        }
    }

    const std::vector<Position>& source = !safeCandidates.empty() ? safeCandidates : allCandidates;
    if (source.empty()) {
        return avoidPosition;
    }

    std::uniform_int_distribution<int> distribution(0, static_cast<int>(source.size()) - 1);
    return source[distribution(randomEngine_)];
}

/**
 * Checks whether two positions are the same.
 * Inputs: left and right positions.
 * Outputs: true when both row and column match.
 */
bool Game::samePosition(const Position& left, const Position& right) const {
    return left.row == right.row && left.col == right.col;
}

/**
 * Checks whether a sentinel can move onto a tile.
 * Inputs: timeline, row, and column.
 * Outputs: true when the sentinel can enter the tile.
 */
bool Game::isSentinelWalkable(Timeline timeline, int row, int col) const {
    if (!map_->inBounds(row, col)) {
        return false;
    }

    char tile = map_->getTile(timeline, row, col);
    if (tile == '#') {
        return false;
    }

    if (tile == 'D' && !switchActive_) {
        return false;
    }

    return true;
}

/**
 * Checks whether two positions share a clear straight line with no blocking tiles.
 * Inputs: timeline and two map positions.
 * Outputs: true when line-of-sight is clear on the same row or column.
 */
bool Game::hasClearLineOfSight(Timeline timeline, const Position& from, const Position& to) const {
    if (from.row == to.row) {
        int step = from.col < to.col ? 1 : -1;
        for (int col = from.col + step; col != to.col; col += step) {
            if (!isSentinelWalkable(timeline, from.row, col)) {
                return false;
            }
        }
        return true;
    }

    if (from.col == to.col) {
        int step = from.row < to.row ? 1 : -1;
        for (int row = from.row + step; row != to.row; row += step) {
            if (!isSentinelWalkable(timeline, row, from.col)) {
                return false;
            }
        }
        return true;
    }

    return false;
}

/**
 * Evaluates whether a sentinel should detect the player.
 * Inputs: sentinel state, timeline, and player position.
 * Outputs: true when line-of-sight or close-area detection is triggered.
 */
bool Game::isSentinelDetectionTriggered(const SentinelState& sentinel, Timeline timeline, const Position& playerPosition) const {
    DifficultySettings settings = getSettings(difficulty_);
    int rowDifference = std::abs(playerPosition.row - sentinel.position.row);
    int colDifference = std::abs(playerPosition.col - sentinel.position.col);

    bool insideCloseScan = rowDifference <= settings.sentinelCloseSquareRadius
        && colDifference <= settings.sentinelCloseSquareRadius;

    return insideCloseScan || hasClearLineOfSight(timeline, sentinel.position, playerPosition);
}

/**
 * Finds the next BFS step from start toward target.
 * Inputs: timeline, start position, target position, and output next-step reference.
 * Outputs: true when a path exists and nextStep is written.
 */
bool Game::findNextStepByBfs(Timeline timeline,
                             const Position& start,
                             const Position& target,
                             Position& nextStep) const {
    if (samePosition(start, target)) {
        return false;
    }

    const std::vector<std::string>& grid = timeline == Timeline::PAST ? map_->getPastGrid() : map_->getPresentGrid();
    if (grid.empty() || grid[0].empty()) {
        return false;
    }

    int height = static_cast<int>(grid.size());
    int width = static_cast<int>(grid[0].size());
    std::vector<std::vector<bool> > visited(height, std::vector<bool>(width, false));
    std::vector<std::vector<Position> > parent(height, std::vector<Position>(width, Position{-1, -1}));
    std::queue<Position> pending;

    pending.push(start);
    visited[start.row][start.col] = true;
    const int directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    while (!pending.empty()) {
        Position current = pending.front();
        pending.pop();

        if (samePosition(current, target)) {
            break;
        }

        for (int index = 0; index < 4; ++index) {
            int nextRow = current.row + directions[index][0];
            int nextCol = current.col + directions[index][1];

            if (!map_->inBounds(nextRow, nextCol)) {
                continue;
            }

            if (visited[nextRow][nextCol]) {
                continue;
            }

            if (!isSentinelWalkable(timeline, nextRow, nextCol)) {
                continue;
            }

            visited[nextRow][nextCol] = true;
            parent[nextRow][nextCol] = current;
            pending.push(Position{nextRow, nextCol});
        }
    }

    if (!visited[target.row][target.col]) {
        return false;
    }

    Position cursor = target;
    Position previous = parent[cursor.row][cursor.col];
    if (previous.row < 0 || previous.col < 0) {
        return false;
    }

    while (!samePosition(previous, start)) {
        cursor = previous;
        previous = parent[cursor.row][cursor.col];
        if (previous.row < 0 || previous.col < 0) {
            return false;
        }
    }

    nextStep = cursor;
    return true;
}

/**
 * Moves an alerted sentinel toward the player for a fixed number of steps.
 * Inputs: sentinel state and step count.
 * Outputs: none.
 */
void Game::moveSentinelTowardPlayer(SentinelState& sentinel, int steps) {
    if (steps <= 0) {
        return;
    }

    for (int step = 0; step < steps; ++step) {
        Position playerPosition = player_->getPosition();
        if (samePosition(sentinel.position, playerPosition)) {
            return;
        }

        Position nextStep = sentinel.position;
        if (!findNextStepByBfs(currentTimeline_, sentinel.position, playerPosition, nextStep)) {
            return;
        }

        sentinel.position = nextStep;
        sentinel.stepCounter++;

        if (samePosition(sentinel.position, playerPosition)) {
            return;
        }
    }
}

/**
 * Converts elapsed real time into available sentinel movement steps.
 * Inputs: timeline and sentinel state.
 * Outputs: number of movement steps that can be executed now.
 */
int Game::sentinelMovesAvailable(Timeline timeline, const SentinelState& sentinel) const {
    if (!sentinel.exists || !sentinel.alerted) {
        return 0;
    }

    DifficultySettings settings = getSettings(difficulty_);
    int periodMs = std::max(100, settings.sentinelMovePeriodMs);
    int movesPerTick = std::max(1, settings.sentinelMovesPerTick);
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point lastMove = sentinelClockForTimeline(timeline);
    long long elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastMove).count();

    if (elapsedMs < periodMs) {
        return 0;
    }

    int ticks = static_cast<int>(elapsedMs / periodMs);
    int steps = ticks * movesPerTick;
    return std::min(steps, 6);
}

/**
 * Updates the current timeline's sentinel chase behavior.
 * Inputs: none.
 * Outputs: none.
 */
void Game::updateSentinelChase() {
    if (mode_ != GameMode::SENTINEL_CHASE || player_ == NULL || map_ == NULL) {
        return;
    }

    Timeline activeTimeline = currentTimeline_;
    SentinelState& sentinel = sentinelForTimeline(activeTimeline);
    if (!sentinel.exists) {
        return;
    }

    Position playerPosition = player_->getPosition();
    if (samePosition(playerPosition, sentinel.position)) {
        sentinelCaught_ = true;
        runningGame_ = false;
        addLog("A sentinel reaches your tile.");
        return;
    }

    DifficultySettings settings = getSettings(difficulty_);
    if (!sentinel.alerted && isSentinelDetectionTriggered(sentinel, activeTimeline, playerPosition)) {
        sentinel.alerted = true;
        sentinelClockForTimeline(activeTimeline) = std::chrono::steady_clock::now()
            - std::chrono::milliseconds(std::max(100, settings.sentinelMovePeriodMs));
        addLog("A sentinel detects you and starts active pursuit.");
    }

    if (!sentinel.alerted) {
        return;
    }

    int steps = sentinelMovesAvailable(activeTimeline, sentinel);
    if (steps <= 0) {
        return;
    }

    moveSentinelTowardPlayer(sentinel, steps);
    sentinelClockForTimeline(activeTimeline) = std::chrono::steady_clock::now();

    if (samePosition(player_->getPosition(), sentinel.position)) {
        sentinelCaught_ = true;
        runningGame_ = false;
        addLog("The sentinel catches you.");
    }
}

/**
 * Updates mode-specific logic after a turn-consuming action.
 * Inputs: whether the action consumed a turn.
 * Outputs: none.
 */
void Game::updateModeAfterAction(bool consumedTurn) {
    if (!runningGame_ || playerWon_ || player_ == NULL) {
        return;
    }

    if (mode_ == GameMode::TIME_TRIAL && consumedTurn && getRemainingTimeSeconds() <= 0) {
        timeExpired_ = true;
        runningGame_ = false;
        addLog("Time is up.");
        return;
    }

    if (mode_ == GameMode::SENTINEL_CHASE) {
        updateSentinelChase();
    }
}

/**
 * Returns the current timeline's sentinel position for rendering.
 * Inputs: none.
 * Outputs: pointer to sentinel position, or NULL when not used.
 */
const Position* Game::currentSentinelPosition() const {
    if (mode_ != GameMode::SENTINEL_CHASE) {
        return NULL;
    }

    const SentinelState& sentinel = sentinelForTimeline(currentTimeline_);
    return sentinel.exists ? &sentinel.position : NULL;
}
