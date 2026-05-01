#ifndef COMMON_H
#define COMMON_H

#include <string>

/**
 * Stores a row and column position on the map grid.
 * Inputs: none.
 * Outputs: a plain data structure used by game classes.
 */
struct Position {
    int row;
    int col;
};

/**
 * Represents the currently active timeline.
 * Inputs: none.
 * Outputs: enum values for Past or Present.
 */
enum class Timeline {
    PAST,
    PRESENT
};

/**
 * Represents the selected difficulty mode.
 * Inputs: none.
 * Outputs: enum values for Easy, Normal, or Hard.
 */
enum class Difficulty {
    EASY,
    NORMAL,
    HARD
};

/**
 * Represents the selected overall game rule set.
 * Inputs: none.
 * Outputs: enum values for Casual, Time Trial, or Sentinel Chase.
 */
enum class GameMode {
    CASUAL,
    TIME_TRIAL,
    SENTINEL_CHASE
};

/**
 * Converts a difficulty enum into display text.
 * Inputs: difficulty value.
 * Outputs: matching uppercase string.
 */
inline std::string difficultyToString(Difficulty difficulty) {
    if (difficulty == Difficulty::EASY) {
        return "EASY";
    }

    if (difficulty == Difficulty::NORMAL) {
        return "NORMAL";
    }

    return "HARD";
}

/**
 * Converts a saved string into a difficulty enum.
 * Inputs: difficulty text and output reference.
 * Outputs: true when conversion succeeds, false otherwise.
 */
inline bool stringToDifficulty(const std::string& text, Difficulty& difficulty) {
    if (text == "EASY") {
        difficulty = Difficulty::EASY;
        return true;
    }

    if (text == "NORMAL") {
        difficulty = Difficulty::NORMAL;
        return true;
    }

    if (text == "HARD") {
        difficulty = Difficulty::HARD;
        return true;
    }

    return false;
}

/**
 * Converts a timeline enum into display text.
 * Inputs: timeline value.
 * Outputs: matching uppercase string.
 */
inline std::string timelineToString(Timeline timeline) {
    return timeline == Timeline::PAST ? "PAST" : "PRESENT";
}

/**
 * Converts a saved string into a timeline enum.
 * Inputs: timeline text and output reference.
 * Outputs: true when conversion succeeds, false otherwise.
 */
inline bool stringToTimeline(const std::string& text, Timeline& timeline) {
    if (text == "PAST") {
        timeline = Timeline::PAST;
        return true;
    }

    if (text == "PRESENT") {
        timeline = Timeline::PRESENT;
        return true;
    }

    return false;
}

/**
 * Converts a game mode enum into display text.
 * Inputs: game mode value.
 * Outputs: matching uppercase string.
 */
inline std::string gameModeToString(GameMode mode) {
    if (mode == GameMode::CASUAL) {
        return "CASUAL";
    }

    if (mode == GameMode::TIME_TRIAL) {
        return "TIME_TRIAL";
    }

    return "SENTINEL_CHASE";
}

/**
 * Converts a saved string into a game mode enum.
 * Inputs: mode text and output reference.
 * Outputs: true when conversion succeeds, false otherwise.
 */
inline bool stringToGameMode(const std::string& text, GameMode& mode) {
    if (text == "CASUAL") {
        mode = GameMode::CASUAL;
        return true;
    }

    if (text == "TIME_TRIAL") {
        mode = GameMode::TIME_TRIAL;
        return true;
    }

    if (text == "SENTINEL_CHASE") {
        mode = GameMode::SENTINEL_CHASE;
        return true;
    }

    return false;
}

#endif
