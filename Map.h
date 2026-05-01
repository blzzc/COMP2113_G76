#ifndef MAP_H
#define MAP_H

#include "Common.h"

#include <string>
#include <vector>

/**
 * Loads, stores, renders, and updates the Past and Present map grids.
 * Inputs: none.
 * Outputs: a map object used by the game loop.
 */
class Map {
private:
    std::vector<std::string> pastGrid_;
    std::vector<std::string> presentGrid_;
    Position startPosition_;
    int height_;
    int width_;

public:
    /**
     * Creates an empty map.
     * Inputs: none.
     * Outputs: empty map object.
     */
    Map();

    /**
     * Loads Past and Present map files from disk.
     * Inputs: file paths for Past and Present, plus an error message reference.
     * Outputs: true when loading succeeds, false otherwise.
     */
    bool loadFromFiles(const std::string& pastPath, const std::string& presentPath, std::string& errorMessage);

    /**
     * Restores the map from already-loaded grid data.
     * Inputs: Past grid, Present grid, and error message reference.
     * Outputs: true when the grids are valid, false otherwise.
     */
    bool setFromSavedGrids(const std::vector<std::string>& pastGrid,
                           const std::vector<std::string>& presentGrid,
                           std::string& errorMessage);

    /**
     * Returns the stored start position.
     * Inputs: none.
     * Outputs: row and column where the player begins.
     */
    Position getStartPosition() const;

    /**
     * Checks whether a position is inside the map boundaries.
     * Inputs: row and column.
     * Outputs: true if the position is valid.
     */
    bool inBounds(int row, int col) const;

    /**
     * Returns the raw tile symbol at a position.
     * Inputs: timeline, row, and column.
     * Outputs: tile character, or '#' when invalid.
     */
    char getTile(Timeline timeline, int row, int col) const;

    /**
     * Updates one tile in one timeline.
     * Inputs: timeline, row, column, and new tile value.
     * Outputs: none.
     */
    void setTile(Timeline timeline, int row, int col, char tile);

    /**
     * Updates the same tile in both timelines.
     * Inputs: row, column, and new tile value.
     * Outputs: none.
     */
    void setTileBoth(int row, int col, char tile);

    /**
     * Checks whether the player can stand on a tile.
     * Inputs: timeline, row, column, whether the player has the key, and whether doors are open.
     * Outputs: true when the tile is currently walkable.
     */
    bool isWalkable(Timeline timeline, int row, int col, bool hasKey, bool switchActive) const;

    /**
     * Explains why movement into a blocked tile failed.
     * Inputs: timeline, row, column, whether the player has the key, and whether doors are open.
     * Outputs: short text reason.
     */
    std::string getBlockReason(Timeline timeline, int row, int col, bool hasKey, bool switchActive) const;

    /**
     * Prints the selected timeline with the player and optional sentinel drawn on top.
     * Inputs: timeline, player position, whether the player has the key, whether doors are open, and optional sentinel pointer.
     * Outputs: terminal map output.
     */
    void render(Timeline timeline,
                const Position& playerPosition,
                bool hasKey,
                bool switchActive,
                const Position* sentinelPosition = NULL) const;

    /**
     * Returns the Past grid for saving.
     * Inputs: none.
     * Outputs: read-only Past map lines.
     */
    const std::vector<std::string>& getPastGrid() const;

    /**
     * Returns the Present grid for saving.
     * Inputs: none.
     * Outputs: read-only Present map lines.
     */
    const std::vector<std::string>& getPresentGrid() const;
};

#endif
