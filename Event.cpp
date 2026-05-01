#include "Event.h"

#include <ctime>
#include <sstream>

/**
 * Creates an empty event.
 * Inputs: none.
 * Outputs: default event object.
 */
Event::Event() : name(""), message(""), hpChange(0), energyChange(0) {
}

/**
 * Creates a fully defined event.
 * Inputs: event name, message, HP change, and energy change.
 * Outputs: initialized event object.
 */
Event::Event(const std::string& eventName, const std::string& eventMessage, int hpDelta, int energyDelta)
    : name(eventName), message(eventMessage), hpChange(hpDelta), energyChange(energyDelta) {
}

/**
 * Creates an event manager with a seeded random engine.
 * Inputs: none.
 * Outputs: initialized event manager.
 */
EventManager::EventManager() : randomEngine_(static_cast<unsigned int>(std::time(NULL))) {
}

/**
 * Frees all allocated events.
 * Inputs: none.
 * Outputs: none.
 */
EventManager::~EventManager() {
    clearPool();
}

/**
 * Deletes all dynamically allocated event objects.
 * Inputs: none.
 * Outputs: none.
 */
void EventManager::clearPool() {
    for (std::size_t index = 0; index < eventPool_.size(); ++index) {
        delete eventPool_[index];
    }

    eventPool_.clear();
}

/**
 * Adds one weighted event entry into the pool.
 * Inputs: event name, message, HP delta, and energy delta.
 * Outputs: none.
 */
void EventManager::addEvent(const std::string& eventName,
                            const std::string& message,
                            int hpDelta,
                            int energyDelta) {
    eventPool_.push_back(new Event(eventName, message, hpDelta, energyDelta));
}

/**
 * Rebuilds the event pool for the selected difficulty.
 * Inputs: difficulty value.
 * Outputs: none.
 */
void EventManager::configure(Difficulty difficulty) {
    clearPool();

    if (difficulty == Difficulty::EASY) {
        addEvent("Herbal Cache", "You find preserved herbs hidden in a cracked urn.", 3, 0);
        addEvent("Herbal Cache", "You find preserved herbs hidden in a cracked urn.", 3, 0);
        addEvent("Echo Crystal", "A glowing shard restores some temporal energy.", 0, 2);
        addEvent("Echo Crystal", "A glowing shard restores some temporal energy.", 0, 2);
        addEvent("Time Distortion", "A weak distortion drains your focus.", 0, -1);
        addEvent("Shade Attack", "A lingering echo lashes out from the shadows.", -2, 0);
        addEvent("Quiet Moment", "Nothing happens, but the silence feels heavy.", 0, 0);
    } else if (difficulty == Difficulty::NORMAL) {
        addEvent("Herbal Cache", "You uncover a small emergency stash.", 2, 0);
        addEvent("Echo Crystal", "A fragment of light recharges your energy.", 0, 1);
        addEvent("Time Distortion", "A temporal wave steals some energy.", 0, -2);
        addEvent("Shade Attack", "A roaming echo claws at you.", -3, 0);
        addEvent("Paradox Shock", "A paradox pulse rattles your body and mind.", -1, -1);
        addEvent("Quiet Moment", "The event tile fades without incident.", 0, 0);
    } else {
        addEvent("Small Remedy", "You scrape together a few useful supplies.", 1, 0);
        addEvent("Echo Crystal", "A tiny spark restores a little energy.", 0, 1);
        addEvent("Time Distortion", "A harsh distortion tears at your reserves.", 0, -2);
        addEvent("Time Distortion", "A harsh distortion tears at your reserves.", 0, -2);
        addEvent("Shade Attack", "A violent echo ambushes you.", -3, 0);
        addEvent("Paradox Shock", "A paradox burst hurts and disorients you.", -2, -1);
        addEvent("Collapsed Memory", "You relive a terrible moment and lose strength.", -4, 0);
    }
}

/**
 * Chooses a random event and applies it to the player.
 * Inputs: player object reference.
 * Outputs: event result text for the game log.
 */
std::string EventManager::triggerRandomEvent(Player& player) {
    if (eventPool_.empty()) {
        return "The strange tile fizzles out without effect.";
    }

    std::uniform_int_distribution<int> distribution(0, static_cast<int>(eventPool_.size()) - 1);
    Event* selectedEvent = eventPool_[distribution(randomEngine_)];

    player.changeHp(selectedEvent->hpChange);
    player.changeEnergy(selectedEvent->energyChange);

    std::ostringstream output;
    output << "Random event - " << selectedEvent->name << ": " << selectedEvent->message;

    if (selectedEvent->hpChange != 0) {
        if (selectedEvent->hpChange > 0) {
            output << " HP +" << selectedEvent->hpChange << ".";
        } else {
            output << " HP " << selectedEvent->hpChange << ".";
        }
    }

    if (selectedEvent->energyChange != 0) {
        if (selectedEvent->energyChange > 0) {
            output << " Energy +" << selectedEvent->energyChange << ".";
        } else {
            output << " Energy " << selectedEvent->energyChange << ".";
        }
    }

    if (selectedEvent->hpChange == 0 && selectedEvent->energyChange == 0) {
        output << " No effect.";
    }

    return output.str();
}
