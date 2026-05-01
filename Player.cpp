#include "Player.h"

#include <sstream>

/**
 * Creates a player with starting HP and energy.
 * Inputs: maximum HP and maximum energy.
 * Outputs: initialized player object.
 */
Player::Player(int maxHp, int maxEnergy)
    : position_{0, 0}, hp_(maxHp), maxHp_(maxHp), energy_(maxEnergy), maxEnergy_(maxEnergy) {
}

/**
 * Sets the player's current map position.
 * Inputs: new row and column position.
 * Outputs: none.
 */
void Player::setPosition(const Position& position) {
    position_ = position;
}

/**
 * Gets the player's current map position.
 * Inputs: none.
 * Outputs: current row and column.
 */
Position Player::getPosition() const {
    return position_;
}

/**
 * Changes HP by a positive or negative amount.
 * Inputs: HP delta value.
 * Outputs: none.
 */
void Player::changeHp(int amount) {
    hp_ += amount;

    if (hp_ > maxHp_) {
        hp_ = maxHp_;
    }

    if (hp_ < 0) {
        hp_ = 0;
    }
}

/**
 * Changes energy by a positive or negative amount.
 * Inputs: energy delta value.
 * Outputs: none.
 */
void Player::changeEnergy(int amount) {
    energy_ += amount;

    if (energy_ > maxEnergy_) {
        energy_ = maxEnergy_;
    }

    if (energy_ < 0) {
        energy_ = 0;
    }
}

/**
 * Overwrites the player's HP and energy values.
 * Inputs: current HP, maximum HP, current energy, maximum energy.
 * Outputs: none.
 */
void Player::setStats(int hp, int maxHp, int energy, int maxEnergy) {
    maxHp_ = maxHp;
    maxEnergy_ = maxEnergy;
    hp_ = hp;
    energy_ = energy;

    if (hp_ > maxHp_) {
        hp_ = maxHp_;
    }

    if (energy_ > maxEnergy_) {
        energy_ = maxEnergy_;
    }
}

/**
 * Adds an item to the inventory.
 * Inputs: item object.
 * Outputs: none.
 */
void Player::addItem(const Item& item) {
    inventory_.push_back(item);
    itemCounts_[item.id]++;
}

/**
 * Rebuilds the inventory from saved item counts.
 * Inputs: saved item-count map.
 * Outputs: none.
 */
void Player::restoreInventory(const std::map<std::string, int>& counts) {
    inventory_.clear();
    itemCounts_.clear();

    for (std::map<std::string, int>::const_iterator it = counts.begin(); it != counts.end(); ++it) {
        itemCounts_[it->first] = it->second;

        for (int i = 0; i < it->second; ++i) {
            inventory_.push_back(createItemById(it->first));
        }
    }
}

/**
 * Checks whether the player owns an item.
 * Inputs: item id string.
 * Outputs: true if the item exists, false otherwise.
 */
bool Player::hasItem(const std::string& itemId) const {
    std::map<std::string, int>::const_iterator it = itemCounts_.find(itemId);
    return it != itemCounts_.end() && it->second > 0;
}

/**
 * Returns current HP.
 * Inputs: none.
 * Outputs: player HP.
 */
int Player::getHp() const {
    return hp_;
}

/**
 * Returns maximum HP.
 * Inputs: none.
 * Outputs: player max HP.
 */
int Player::getMaxHp() const {
    return maxHp_;
}

/**
 * Returns current energy.
 * Inputs: none.
 * Outputs: player energy.
 */
int Player::getEnergy() const {
    return energy_;
}

/**
 * Returns maximum energy.
 * Inputs: none.
 * Outputs: player max energy.
 */
int Player::getMaxEnergy() const {
    return maxEnergy_;
}

/**
 * Checks whether the player is still alive.
 * Inputs: none.
 * Outputs: true when HP is above zero.
 */
bool Player::isAlive() const {
    return hp_ > 0;
}

/**
 * Returns a read-only view of item counts.
 * Inputs: none.
 * Outputs: inventory count map.
 */
const std::map<std::string, int>& Player::getItemCounts() const {
    return itemCounts_;
}

/**
 * Formats the inventory as terminal-friendly text.
 * Inputs: none.
 * Outputs: summary string.
 */
std::string Player::inventorySummary() const {
    if (itemCounts_.empty()) {
        return "Empty";
    }

    std::ostringstream output;
    bool first = true;

    for (std::map<std::string, int>::const_iterator it = itemCounts_.begin(); it != itemCounts_.end(); ++it) {
        if (!first) {
            output << ", ";
        }

        Item item = createItemById(it->first);
        output << item.name << " x" << it->second;
        first = false;
    }

    return output.str();
}
