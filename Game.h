#ifndef GAME_H
#define GAME_H

#include "Common.h"
#include "Event.h"
#include "Map.h"
#include "MiniGame.h"
#include "Player.h"
#include "SaveSystem.h"

#include <chrono>
#include <random>
#include <string>
#include <vector>

/**
 * Coordinates menus, gameplay, saving, loading, and puzzle logic.
 * Inputs: none.
 * Outputs: a complete terminal game controller.
 */
class Game {
private:
    /**
     * Stores the gameplay values associated with each difficulty.
     * Inputs: none.
     * Outputs: a simple settings structure.
     */
    struct DifficultySettings {
        int startHp;
        int startEnergy;
        int switchCost;
        int trapMinDamage;
        int trapMaxDamage;
        bool showHints;
        int timeLimitSeconds;
        int sentinelCloseSquareRadius;
        int sentinelMovePeriodMs;
        int sentinelMovesPerTick;
    };

    /**
     * Stores one sentinel's live runtime state.
     * Inputs: none.
     * Outputs: a simple chase-state structure.
     */
    struct SentinelState {
        bool exists;
        Position position;
        bool alerted;
        int stepCounter;
    };

    Player* player_;
    Map* map_;
    EventManager* eventManager_;
    MiniGameManager* miniGameManager_;
    bool switchActive_;
    bool runningGame_;
    bool playerWon_;
    bool timeExpired_;
    bool sentinelCaught_;
    Timeline currentTimeline_;
    Difficulty difficulty_;
    GameMode mode_;
    int moves_;
    int timeLimitSeconds_;
    std::string baseDirectory_;
    std::vector<std::string> eventLog_;
    SentinelState pastSentinel_;
    SentinelState presentSentinel_;
    std::chrono::steady_clock::time_point timerStart_;
    std::chrono::steady_clock::time_point pastSentinelLastMove_;
    std::chrono::steady_clock::time_point presentSentinelLastMove_;
    std::mt19937 randomEngine_;

    void clearResources();
    void showMainMenu() const;
    void showHelp() const;
    bool promptForDifficulty(Difficulty& difficulty) const;
    bool promptForGameMode(GameMode& mode) const;
    bool startNewGame(Difficulty difficulty, GameMode mode);
    bool loadSavedGame();
    bool applySaveData(const SaveData& data, std::string& errorMessage);
    void gameLoop();
    void renderGame() const;
    void processCommand(char command);
    bool attemptMove(int rowDelta, int colDelta);
    bool attemptTimelineSwitch();
    void handleTile(char tile);
    void addLog(const std::string& message);
    void saveCurrentGame();
    DifficultySettings getSettings(Difficulty difficulty) const;
    void getMapPaths(Difficulty difficulty, std::string& pastPath, std::string& presentPath) const;
    std::string resolveProjectPath(const std::string& relativePath) const;
    void clearScreen() const;
    void waitForEnter() const;
    int randomInt(int minimum, int maximum);
    std::string objectiveText() const;
    void applyRunVariation();
    void initializeModeState();
    int getRemainingTimeSeconds() const;
    void applyTimePenalty(int seconds, const std::string& reason);
    SentinelState& sentinelForTimeline(Timeline timeline);
    const SentinelState& sentinelForTimeline(Timeline timeline) const;
    std::chrono::steady_clock::time_point& sentinelClockForTimeline(Timeline timeline);
    const std::chrono::steady_clock::time_point& sentinelClockForTimeline(Timeline timeline) const;
    void initializeSentinels();
    Position chooseSentinelSpawn(Timeline timeline, const Position& avoidPosition);
    bool samePosition(const Position& left, const Position& right) const;
    bool isSentinelWalkable(Timeline timeline, int row, int col) const;
    bool hasClearLineOfSight(Timeline timeline, const Position& from, const Position& to) const;
    bool isSentinelDetectionTriggered(const SentinelState& sentinel, Timeline timeline, const Position& playerPosition) const;
    bool findNextStepByBfs(Timeline timeline, const Position& start, const Position& target, Position& nextStep) const;
    void moveSentinelTowardPlayer(SentinelState& sentinel, int steps);
    int sentinelMovesAvailable(Timeline timeline, const SentinelState& sentinel) const;
    void updateSentinelChase();
    void updateModeAfterAction(bool consumedTurn);
    const Position* currentSentinelPosition() const;

public:
    Game(const std::string& baseDirectory);
    ~Game();
    void run();
};

#endif
