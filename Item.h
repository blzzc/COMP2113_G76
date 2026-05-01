#ifndef ITEM_H
#define ITEM_H

#include <string>

/**
 * Represents a simple inventory item.
 * Inputs: none.
 * Outputs: an object containing an item id, name, and description.
 */
class Item {
public:
    std::string id;
    std::string name;
    std::string description;

    /**
     * Creates an empty item.
     * Inputs: none.
     * Outputs: default item object.
     */
    Item();

    /**
     * Creates an item with full information.
     * Inputs: item id, item name, and item description.
     * Outputs: initialized item object.
     */
    Item(const std::string& itemId, const std::string& itemName, const std::string& itemDescription);
};

/**
 * Builds a known game item from its id.
 * Inputs: item id string.
 * Outputs: a matching item object.
 */
Item createItemById(const std::string& itemId);

#endif
