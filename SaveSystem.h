#ifndef SAVESYSTEM_H
#define SAVESYSTEM_H

#include "Common.h"

#include <map>
#include <string>
#include <vector>

/**
 * Stores one sentinel state inside a save file.
 * Inputs: none.
 * Outputs: a plain data structure for sentinel persistence.
 */
struct SentinelSaveData {
    bool exists;
    Position position;
    bool alerted;
    int stepCounter;
};

/**
 * Stores the full game state needed for saving and loading.
 * Inputs: none.
 * Outputs: a plain data structure for file I/O.
 */
struct SaveData {
    Difficulty difficulty;
    GameMode mode;
    Timeline timeline;
    Position position;
    int hp;
    int maxHp;
    int energy;
    int maxEnergy;
    bool switchActive;
    int moves;
    int remainingTimeSeconds;
    SentinelSaveData pastSentinel;
    SentinelSaveData presentSentinel;
    std::map<std::string, int> inventoryCounts;
    std::vector<std::string> pastGrid;
    std::vector<std::string> presentGrid;
};

/**
 * Handles saving and loading the game using text files.
 * Inputs: none.
 * Outputs: static helper functions for file I/O.
 */
class SaveSystem {
public:
    /**
     * Writes game state into a save file.
     * Inputs: save path, save data, and error message reference.
     * Outputs: true when the save is written successfully.
     */
    static bool saveGame(const std::string& path, const SaveData& data, std::string& errorMessage);

    /**
     * Reads game state from a save file.
     * Inputs: save path, output save data, and error message reference.
     * Outputs: true when the save is read successfully.
     */
    static bool loadGame(const std::string& path, SaveData& data, std::string& errorMessage);
};

#endif
