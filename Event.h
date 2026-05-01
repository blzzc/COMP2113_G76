#ifndef EVENT_H
#define EVENT_H

#include "Common.h"
#include "Player.h"

#include <random>
#include <string>
#include <vector>

/**
 * Represents one possible random event result.
 * Inputs: none.
 * Outputs: a data object with event effects.
 */
struct Event {
    std::string name;
    std::string message;
    int hpChange;
    int energyChange;

    /**
     * Creates an empty event.
     * Inputs: none.
     * Outputs: default event object.
     */
    Event();

    /**
     * Creates a fully defined event.
     * Inputs: event name, message, HP change, and energy change.
     * Outputs: initialized event object.
     */
    Event(const std::string& eventName, const std::string& eventMessage, int hpDelta, int energyDelta);
};

/**
 * Manages the random-event pool and applies event outcomes.
 * Inputs: none.
 * Outputs: an event manager object with dynamically allocated events.
 */
class EventManager {
private:
    std::vector<Event*> eventPool_;
    std::mt19937 randomEngine_;

    /**
     * Deletes all dynamically allocated event objects.
     * Inputs: none.
     * Outputs: none.
     */
    void clearPool();

    /**
     * Adds one weighted event entry into the pool.
     * Inputs: event name, message, HP delta, and energy delta.
     * Outputs: none.
     */
    void addEvent(const std::string& eventName, const std::string& message, int hpDelta, int energyDelta);

public:
    /**
     * Creates an event manager with a seeded random engine.
     * Inputs: none.
     * Outputs: initialized event manager.
     */
    EventManager();

    /**
     * Frees all allocated events.
     * Inputs: none.
     * Outputs: none.
     */
    ~EventManager();

    /**
     * Rebuilds the event pool for the selected difficulty.
     * Inputs: difficulty value.
     * Outputs: none.
     */
    void configure(Difficulty difficulty);

    /**
     * Chooses a random event and applies it to the player.
     * Inputs: player object reference.
     * Outputs: event result text for the game log.
     */
    std::string triggerRandomEvent(Player& player);
};

#endif
