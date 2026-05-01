#include "SaveSystem.h"

#include <fstream>

/**
 * Reads the next keyword from a save file.
 * Inputs: file stream, expected keyword, and error message reference.
 * Outputs: true when the keyword matches.
 */
static bool expectLabel(std::ifstream& input, const std::string& expected, std::string& errorMessage) {
    std::string label;

    if (!(input >> label)) {
        errorMessage = "Save file is empty. Start a new game and press V to create a save.";
        return false;
    }

    if (label != expected) {
        errorMessage = "Save file is corrupted near label: " + expected;
        return false;
    }

    return true;
}

/**
 * Writes game state into a save file.
 * Inputs: save path, save data, and error message reference.
 * Outputs: true when the save is written successfully.
 */
bool SaveSystem::saveGame(const std::string& path, const SaveData& data, std::string& errorMessage) {
    std::ofstream output(path.c_str());

    if (!output.is_open()) {
        errorMessage = "Unable to open save file for writing: " + path;
        return false;
    }

    output << "difficulty " << difficultyToString(data.difficulty) << "\n";
    output << "mode " << gameModeToString(data.mode) << "\n";
    output << "timeline " << timelineToString(data.timeline) << "\n";
    output << "row " << data.position.row << "\n";
    output << "col " << data.position.col << "\n";
    output << "hp " << data.hp << "\n";
    output << "max_hp " << data.maxHp << "\n";
    output << "energy " << data.energy << "\n";
    output << "max_energy " << data.maxEnergy << "\n";
    output << "switch_active " << (data.switchActive ? 1 : 0) << "\n";
    output << "moves " << data.moves << "\n";
    output << "remaining_time " << data.remainingTimeSeconds << "\n";
    output << "past_sentinel " << (data.pastSentinel.exists ? 1 : 0) << " "
           << data.pastSentinel.position.row << " "
           << data.pastSentinel.position.col << " "
           << (data.pastSentinel.alerted ? 1 : 0) << " "
           << data.pastSentinel.stepCounter << "\n";
    output << "present_sentinel " << (data.presentSentinel.exists ? 1 : 0) << " "
           << data.presentSentinel.position.row << " "
           << data.presentSentinel.position.col << " "
           << (data.presentSentinel.alerted ? 1 : 0) << " "
           << data.presentSentinel.stepCounter << "\n";
    output << "inventory_entries " << data.inventoryCounts.size() << "\n";

    for (std::map<std::string, int>::const_iterator it = data.inventoryCounts.begin(); it != data.inventoryCounts.end(); ++it) {
        output << "item " << it->first << " " << it->second << "\n";
    }

    output << "past_rows " << data.pastGrid.size() << "\n";
    for (std::size_t row = 0; row < data.pastGrid.size(); ++row) {
        output << data.pastGrid[row] << "\n";
    }

    output << "present_rows " << data.presentGrid.size() << "\n";
    for (std::size_t row = 0; row < data.presentGrid.size(); ++row) {
        output << data.presentGrid[row] << "\n";
    }

    return true;
}

/**
 * Reads game state from a save file.
 * Inputs: save path, output save data, and error message reference.
 * Outputs: true when the save is read successfully.
 */
bool SaveSystem::loadGame(const std::string& path, SaveData& data, std::string& errorMessage) {
    std::ifstream input(path.c_str());

    if (!input.is_open()) {
        errorMessage = "No save file found at " + path;
        return false;
    }

    std::string difficultyText;
    std::string modeText;
    std::string timelineText;

    if (!expectLabel(input, "difficulty", errorMessage) || !(input >> difficultyText)) {
        return false;
    }

    if (!stringToDifficulty(difficultyText, data.difficulty)) {
        errorMessage = "Invalid difficulty in save file.";
        return false;
    }

    if (!expectLabel(input, "mode", errorMessage) || !(input >> modeText)) {
        return false;
    }

    if (!stringToGameMode(modeText, data.mode)) {
        errorMessage = "Invalid game mode in save file.";
        return false;
    }

    if (!expectLabel(input, "timeline", errorMessage) || !(input >> timelineText)) {
        return false;
    }

    if (!stringToTimeline(timelineText, data.timeline)) {
        errorMessage = "Invalid timeline in save file.";
        return false;
    }

    if (!expectLabel(input, "row", errorMessage) || !(input >> data.position.row)) {
        return false;
    }

    if (!expectLabel(input, "col", errorMessage) || !(input >> data.position.col)) {
        return false;
    }

    if (!expectLabel(input, "hp", errorMessage) || !(input >> data.hp)) {
        return false;
    }

    if (!expectLabel(input, "max_hp", errorMessage) || !(input >> data.maxHp)) {
        return false;
    }

    if (!expectLabel(input, "energy", errorMessage) || !(input >> data.energy)) {
        return false;
    }

    if (!expectLabel(input, "max_energy", errorMessage) || !(input >> data.maxEnergy)) {
        return false;
    }

    int switchFlag = 0;
    if (!expectLabel(input, "switch_active", errorMessage) || !(input >> switchFlag)) {
        return false;
    }
    data.switchActive = switchFlag != 0;

    if (!expectLabel(input, "moves", errorMessage) || !(input >> data.moves)) {
        return false;
    }

    if (!expectLabel(input, "remaining_time", errorMessage) || !(input >> data.remainingTimeSeconds)) {
        return false;
    }

    int existsFlag = 0;
    int alertedFlag = 0;
    if (!expectLabel(input, "past_sentinel", errorMessage)
        || !(input >> existsFlag >> data.pastSentinel.position.row >> data.pastSentinel.position.col >> alertedFlag >> data.pastSentinel.stepCounter)) {
        return false;
    }
    data.pastSentinel.exists = existsFlag != 0;
    data.pastSentinel.alerted = alertedFlag != 0;

    if (!expectLabel(input, "present_sentinel", errorMessage)
        || !(input >> existsFlag >> data.presentSentinel.position.row >> data.presentSentinel.position.col >> alertedFlag >> data.presentSentinel.stepCounter)) {
        return false;
    }
    data.presentSentinel.exists = existsFlag != 0;
    data.presentSentinel.alerted = alertedFlag != 0;

    std::size_t inventoryEntryCount = 0;
    if (!expectLabel(input, "inventory_entries", errorMessage) || !(input >> inventoryEntryCount)) {
        return false;
    }

    data.inventoryCounts.clear();
    for (std::size_t index = 0; index < inventoryEntryCount; ++index) {
        std::string label;
        std::string itemId;
        int count = 0;

        if (!(input >> label >> itemId >> count) || label != "item") {
            errorMessage = "Inventory data is corrupted.";
            return false;
        }

        data.inventoryCounts[itemId] = count;
    }

    std::size_t pastRowCount = 0;
    if (!expectLabel(input, "past_rows", errorMessage) || !(input >> pastRowCount)) {
        return false;
    }

    data.pastGrid.clear();
    for (std::size_t row = 0; row < pastRowCount; ++row) {
        std::string line;
        if (!(input >> line)) {
            errorMessage = "Past map data is incomplete.";
            return false;
        }
        data.pastGrid.push_back(line);
    }

    std::size_t presentRowCount = 0;
    if (!expectLabel(input, "present_rows", errorMessage) || !(input >> presentRowCount)) {
        return false;
    }

    data.presentGrid.clear();
    for (std::size_t row = 0; row < presentRowCount; ++row) {
        std::string line;
        if (!(input >> line)) {
            errorMessage = "Present map data is incomplete.";
            return false;
        }
        data.presentGrid.push_back(line);
    }

    return true;
}
