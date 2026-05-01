#ifndef PLAYER_H
#define PLAYER_H

#include "Common.h"
#include "Item.h"

#include <map>
#include <string>
#include <vector>

/**
 * Stores player statistics, location, and inventory data.
 * Inputs: none.
 * Outputs: a reusable player object for gameplay logic.
 */
class Player {
private:
    Position position_;
    int hp_;
    int maxHp_;
    int energy_;
    int maxEnergy_;
    std::vector<Item> inventory_;
    std::map<std::string, int> itemCounts_;

public:
    /**
     * Creates a player with starting HP and energy.
     * Inputs: maximum HP and maximum energy.
     * Outputs: initialized player object.
     */
    Player(int maxHp, int maxEnergy);

    /**
     * Sets the player's current map position.
     * Inputs: new row and column position.
     * Outputs: none.
     */
    void setPosition(const Position& position);

    /**
     * Gets the player's current map position.
     * Inputs: none.
     * Outputs: current row and column.
     */
    Position getPosition() const;

    /**
     * Changes HP by a positive or negative amount.
     * Inputs: HP delta value.
     * Outputs: none.
     */
    void changeHp(int amount);

    /**
     * Changes energy by a positive or negative amount.
     * Inputs: energy delta value.
     * Outputs: none.
     */
    void changeEnergy(int amount);

    /**
     * Overwrites the player's HP and energy values.
     * Inputs: current HP, maximum HP, current energy, maximum energy.
     * Outputs: none.
     */
    void setStats(int hp, int maxHp, int energy, int maxEnergy);

    /**
     * Adds an item to the inventory.
     * Inputs: item object.
     * Outputs: none.
     */
    void addItem(const Item& item);

    /**
     * Rebuilds the inventory from saved item counts.
     * Inputs: saved item-count map.
     * Outputs: none.
     */
    void restoreInventory(const std::map<std::string, int>& counts);

    /**
     * Checks whether the player owns an item.
     * Inputs: item id string.
     * Outputs: true if the item exists, false otherwise.
     */
    bool hasItem(const std::string& itemId) const;

    /**
     * Returns current HP.
     * Inputs: none.
     * Outputs: player HP.
     */
    int getHp() const;

    /**
     * Returns maximum HP.
     * Inputs: none.
     * Outputs: player max HP.
     */
    int getMaxHp() const;

    /**
     * Returns current energy.
     * Inputs: none.
     * Outputs: player energy.
     */
    int getEnergy() const;

    /**
     * Returns maximum energy.
     * Inputs: none.
     * Outputs: player max energy.
     */
    int getMaxEnergy() const;

    /**
     * Checks whether the player is still alive.
     * Inputs: none.
     * Outputs: true when HP is above zero.
     */
    bool isAlive() const;

    /**
     * Returns a read-only view of item counts.
     * Inputs: none.
     * Outputs: inventory count map.
     */
    const std::map<std::string, int>& getItemCounts() const;

    /**
     * Formats the inventory as terminal-friendly text.
     * Inputs: none.
     * Outputs: summary string.
     */
    std::string inventorySummary() const;
};

#endif
