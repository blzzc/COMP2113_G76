#include "Item.h"

/**
 * Creates an empty item.
 * Inputs: none.
 * Outputs: default item object.
 */
Item::Item() : id(""), name(""), description("") {
}

/**
 * Creates an item with full information.
 * Inputs: item id, item name, and item description.
 * Outputs: initialized item object.
 */
Item::Item(const std::string& itemId, const std::string& itemName, const std::string& itemDescription)
    : id(itemId), name(itemName), description(itemDescription) {
}

/**
 * Builds a known game item from its id.
 * Inputs: item id string.
 * Outputs: a matching item object.
 */
Item createItemById(const std::string& itemId) {
    if (itemId == "TimeKey") {
        return Item("TimeKey", "Time Key", "An ancient key that resonates across both timelines.");
    }

    return Item(itemId, itemId, "A mysterious item recovered from the ruin.");
}
