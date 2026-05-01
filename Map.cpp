#include "Map.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

/**
 * Loads a text map file into a string vector.
 * Inputs: file path and output vector.
 * Outputs: true when the file is read successfully.
 */
static bool loadGridFile(const std::string& path, std::vector<std::string>& grid) {
    std::ifstream input(path.c_str());

    if (!input.is_open()) {
        return false;
    }

    grid.clear();
    std::string line;

    while (std::getline(input, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r') {
            line.erase(line.size() - 1);
        }

        if (!line.empty()) {
            grid.push_back(line);
        }
    }

    return !grid.empty();
}

/**
 * Finds the player start tile inside a grid.
 * Inputs: grid lines and output position.
 * Outputs: true when a start tile is found.
 */
static bool findStartPosition(const std::vector<std::string>& grid, Position& position) {
    for (std::size_t row = 0; row < grid.size(); ++row) {
        for (std::size_t col = 0; col < grid[row].size(); ++col) {
            if (grid[row][col] == 'P') {
                position.row = static_cast<int>(row);
                position.col = static_cast<int>(col);
                return true;
            }
        }
    }

    return false;
}

/**
 * Removes the start tile marker so the player can move normally.
 * Inputs: grid lines and start position.
 * Outputs: none.
 */
static void clearStartMarker(std::vector<std::string>& grid, const Position& position) {
    if (position.row >= 0 &&
        position.row < static_cast<int>(grid.size()) &&
        position.col >= 0 &&
        position.col < static_cast<int>(grid[position.row].size())) {
        grid[position.row][position.col] = '.';
    }
}

/**
 * Returns the ANSI color sequence for a tile.
 * Inputs: tile character and whether the door switch is active.
 * Outputs: ANSI color code as text.
 */
static std::string tileColor(char tile, bool switchActive) {
    if (tile == '#') {
        return "\033[1;37;100m";
    }

    if (tile == '.') {
        return "\033[0;37m";
    }

    if (tile == 'P') {
        return "\033[1;30;106m";
    }

    if (tile == 'G') {
        return "\033[1;97;41m";
    }

    if (tile == 'K') {
        return "\033[1;30;103m";
    }

    if (tile == 'S') {
        return "\033[1;97;45m";
    }

    if (tile == '?') {
        return "\033[1;97;44m";
    }

    if (tile == '^' || tile == 'E') {
        return "\033[1;97;41m";
    }

    if (tile == 'X') {
        return "\033[1;30;102m";
    }

    if (tile == 'D' && switchActive) {
        return "\033[1;30;102m";
    }

    if (tile == 'D') {
        return "\033[1;30;103m";
    }

    if (tile == '/') {
        return "\033[1;30;102m";
    }

    return "\033[0m";
}

/**
 * Builds a colored cell string for one map tile.
 * Inputs: tile character and whether the door switch is active.
 * Outputs: formatted colored cell text.
 */
static std::string formatTileCell(char tile, bool switchActive) {
    std::ostringstream output;
    output << tileColor(tile, switchActive) << tile << "\033[0m";
    return output.str();
}

/**
 * Creates an empty map.
 * Inputs: none.
 * Outputs: empty map object.
 */
Map::Map() : startPosition_{0, 0}, height_(0), width_(0) {
}

/**
 * Loads Past and Present map files from disk.
 * Inputs: file paths for Past and Present, plus an error message reference.
 * Outputs: true when loading succeeds, false otherwise.
 */
bool Map::loadFromFiles(const std::string& pastPath, const std::string& presentPath, std::string& errorMessage) {
    std::vector<std::string> pastGrid;
    std::vector<std::string> presentGrid;

    if (!loadGridFile(pastPath, pastGrid)) {
        errorMessage = "Unable to read Past map file: " + pastPath;
        return false;
    }

    if (!loadGridFile(presentPath, presentGrid)) {
        errorMessage = "Unable to read Present map file: " + presentPath;
        return false;
    }

    return setFromSavedGrids(pastGrid, presentGrid, errorMessage);
}

/**
 * Restores the map from already-loaded grid data.
 * Inputs: Past grid, Present grid, and error message reference.
 * Outputs: true when the grids are valid, false otherwise.
 */
bool Map::setFromSavedGrids(const std::vector<std::string>& pastGrid,
                            const std::vector<std::string>& presentGrid,
                            std::string& errorMessage) {
    if (pastGrid.empty() || presentGrid.empty()) {
        errorMessage = "Map data is empty.";
        return false;
    }

    if (pastGrid.size() != presentGrid.size()) {
        errorMessage = "Past and Present maps must have the same height.";
        return false;
    }

    std::size_t width = pastGrid[0].size();

    for (std::size_t row = 0; row < pastGrid.size(); ++row) {
        if (pastGrid[row].size() != width || presentGrid[row].size() != width) {
            errorMessage = "All map rows must have the same width in both timelines.";
            return false;
        }
    }

    pastGrid_ = pastGrid;
    presentGrid_ = presentGrid;
    height_ = static_cast<int>(pastGrid_.size());
    width_ = static_cast<int>(width);

    Position pastStart;
    Position presentStart;
    bool hasPastStart = findStartPosition(pastGrid_, pastStart);
    bool hasPresentStart = findStartPosition(presentGrid_, presentStart);

    if (hasPastStart && hasPresentStart) {
        if (pastStart.row != presentStart.row || pastStart.col != presentStart.col) {
            errorMessage = "Start position must match in both timelines.";
            return false;
        }

        startPosition_ = pastStart;
        clearStartMarker(pastGrid_, pastStart);
        clearStartMarker(presentGrid_, presentStart);
    } else {
        startPosition_.row = 1;
        startPosition_.col = 1;
    }

    return true;
}

/**
 * Returns the stored start position.
 * Inputs: none.
 * Outputs: row and column where the player begins.
 */
Position Map::getStartPosition() const {
    return startPosition_;
}

/**
 * Checks whether a position is inside the map boundaries.
 * Inputs: row and column.
 * Outputs: true if the position is valid.
 */
bool Map::inBounds(int row, int col) const {
    return row >= 0 && row < height_ && col >= 0 && col < width_;
}

/**
 * Returns the raw tile symbol at a position.
 * Inputs: timeline, row, and column.
 * Outputs: tile character, or '#' when invalid.
 */
char Map::getTile(Timeline timeline, int row, int col) const {
    if (!inBounds(row, col)) {
        return '#';
    }

    const std::vector<std::string>& grid = timeline == Timeline::PAST ? pastGrid_ : presentGrid_;
    return grid[row][col];
}

/**
 * Updates one tile in one timeline.
 * Inputs: timeline, row, column, and new tile value.
 * Outputs: none.
 */
void Map::setTile(Timeline timeline, int row, int col, char tile) {
    if (!inBounds(row, col)) {
        return;
    }

    std::vector<std::string>& grid = timeline == Timeline::PAST ? pastGrid_ : presentGrid_;
    grid[row][col] = tile;
}

/**
 * Updates the same tile in both timelines.
 * Inputs: row, column, and new tile value.
 * Outputs: none.
 */
void Map::setTileBoth(int row, int col, char tile) {
    if (!inBounds(row, col)) {
        return;
    }

    pastGrid_[row][col] = tile;
    presentGrid_[row][col] = tile;
}

/**
 * Checks whether the player can stand on a tile.
 * Inputs: timeline, row, column, whether the player has the key, and whether doors are open.
 * Outputs: true when the tile is currently walkable.
 */
bool Map::isWalkable(Timeline timeline, int row, int col, bool hasKey, bool switchActive) const {
    const char tile = getTile(timeline, row, col);

    // 墙壁永远不可走
    if (tile == '#') {
        return false;
    }

    // 门由开关控制
    if (tile == 'D') {
        return switchActive;
    }

    // 出口需要钥匙
    if (tile == 'X') {
        return hasKey;
    }

    // 其他地块默认可通行
    return true;
}

/**
 * Explains why movement into a blocked tile failed.
 * Inputs: timeline, row, column, whether the player has the key, and whether doors are open.
 * Outputs: short text reason.
 */
std::string Map::getBlockReason(Timeline timeline, int row, int col, bool hasKey, bool switchActive) const {
    if (!inBounds(row, col)) {
        return "A distorted wall blocks your path.";
    }

    const char tile = getTile(timeline, row, col);

    if (tile == '#') {
        return "A solid wall blocks the way in this timeline.";
    }

    if (tile == 'D' && !switchActive) {
        return "The ancient door is sealed. Find and activate the switch.";
    }

    if (tile == 'X' && !hasKey) {
        return "The exit rejects you. You still need the Time Key.";
    }

    return "Something prevents movement.";
}

/**
 * Prints the selected timeline with the player and optional sentinel drawn on top.
 * Inputs: timeline, player position, whether the player has the key, whether doors are open, and optional sentinel pointer.
 * Outputs: terminal map output.
 */
void Map::render(Timeline timeline,
                 const Position& playerPosition,
                 bool hasKey,
                 bool switchActive,
                 const Position* sentinelPosition) const {
    const std::vector<std::string>& grid = timeline == Timeline::PAST ? pastGrid_ : presentGrid_;
    std::cout << "    ";

    for (int col = 0; col < width_; ++col) {
        std::cout << std::setw(2) << col % 10 << ' ';
    }

    std::cout << '\n';

    for (int row = 0; row < height_; ++row) {
        std::cout << std::setw(2) << row % 100 << " |";

        for (int col = 0; col < width_; ++col) {
            if (playerPosition.row == row && playerPosition.col == col) {
                std::cout << ' ' << formatTileCell('P', switchActive) << ' ';
                continue;
            }

            if (sentinelPosition != NULL && sentinelPosition->row == row && sentinelPosition->col == col) {
                std::cout << ' ' << formatTileCell('G', switchActive) << ' ';
                continue;
            }

            char tile = grid[row][col];

            if (tile == 'D' && switchActive) {
                tile = '/';
            }

            if (tile == 'X' && !hasKey) {
                tile = 'X';
            }

            std::cout << ' ' << formatTileCell(tile, switchActive) << ' ';
        }

        std::cout << '\n';
    }
}

/**
 * Returns the Past grid for saving.
 * Inputs: none.
 * Outputs: read-only Past map lines.
 */
const std::vector<std::string>& Map::getPastGrid() const {
    return pastGrid_;
}

/**
 * Returns the Present grid for saving.
 * Inputs: none.
 * Outputs: read-only Present map lines.
 */
const std::vector<std::string>& Map::getPresentGrid() const {
    return presentGrid_;
}
