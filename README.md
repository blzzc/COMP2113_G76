# Echoes of Time

## Team Members
- Member 1: ____________________
- Member 2: ____________________
- Member 3: ____________________

## Team Contribution Plan (3 Members)
- Member 1 (Core Engine): game loop, timeline switching, movement/collision, win/lose checks.
- Member 2 (World & Data): map design/balance, file I/O save-load, difficulty tuning.
- Member 3 (Features & UX): random events/mini-games/sentinel, HUD color polish, documentation/testing.

## Contribution Evidence (Commitment)
Each member should have meaningful commits in their own feature area.  
Suggested commit message format:
- `[M1] Implement timeline switch validation`
- `[M2] Rebalance hard maps and fix door routing`
- `[M3] Add real-time sentinel chase update`

Before final submission, include these outputs in your report/appendix:
```bash
git shortlog -sne
git log --oneline --decorate --graph
git log --name-only --author="member_name_or_id"
```

Recommended minimum per member:
- At least 5-8 non-trivial commits
- Commits distributed across development period (not all in one day)
- Evidence of code + test/debug/documentation contributions

## Game Description
**Echoes of Time** is a terminal-based puzzle adventure game written in standard C++. The player is trapped inside a ruined structure that exists in two linked timelines: the **Past** and the **Present**. Some pathways only exist in one timeline, while doors, traps, keys, and hazards change how the player explores the ruin. The main objective is to collect the **Time Key**, open the sealed route, and reach the final exit.

## Game Modes
1. **Casual Mode**
- Original puzzle-adventure rules.
- `?` tiles trigger normal random events.

2. **Time Trial Mode**
- Adds a countdown timer.
- `?` tiles launch one of four random mini-games.
- Mini-game win/lose both apply time penalty (lose is harsher).
- Escape through `X` before time reaches zero.

3. **Sentinel Chase Mode**
- Each timeline has one sentinel (`G`).
- Sentinel in current timeline can detect and chase the player.
- Switching timeline pauses the previous timeline sentinel and activates the other one.
- Avoid being caught; open route and reach `X` to win.

## Features Implemented
- Two linked world states: Past and Present
- Three gameplay modes: Casual / Time Trial / Sentinel Chase
- ASCII grid map rendered in terminal
- ANSI color highlighting for map symbols and HUD
- HP and energy bars for clearer status display
- Player movement using `W`, `A`, `S`, `D`
- Timeline switching using `T`
- Key, switch, door, and exit puzzle system
- Random event system
- Four random terminal mini-games for Time Trial mode
- Sentinel detection + chase system in Sentinel Chase mode
- Save/load system using `data/save.txt`
- Three difficulty modes: Easy, Normal, Hard
- Win and lose conditions

## How the Required Coding Elements Support the Game
### 1. Random Events
- Casual/Sentinel modes: stepping on `?` triggers random effects via `EventManager`.
- Time Trial mode: stepping on `?` launches a random mini-game from four options.

### 2. Data Structures
The project uses standard C++ data structures:
- `vector<string>` for map layouts
- `vector<Item>` for inventory
- `map<string, int>` for item counts
- `vector<string>` for recent event logs
- `struct`/`class` for `Player`, `Map`, `Event`, `Game`, `MiniGameResult`, `SaveData`

### 3. Dynamic Memory Management
Dynamic memory is used explicitly with `new`/`delete`:
- `Game` dynamically allocates `Player`, `Map`, `EventManager`, and `MiniGameManager`
- `EventManager` dynamically allocates `Event` objects in its weighted pool

### 4. File Input / Output
File I/O is used for:
- Loading maps from `maps/`
- Saving/loading full game state to/from `data/save.txt`

### 5. Multiple Files
The project is split into multiple headers and source files for modular collaboration:
- `Game`, `Player`, `Map`, `EventManager`, `MiniGameManager`, `SaveSystem`, `Item`

### Difficulty Modes
Difficulty affects gameplay meaningfully:
- Starting HP / energy
- Timeline switch cost
- Trap and hazard damage
- Time limit (Time Trial mode)
- Sentinel detection/move aggressiveness (Sentinel mode)
- Hint visibility

## Non-Standard Libraries Used
None. The project uses only the C++ standard library and ANSI terminal escape codes (no external installation required).

## Compilation Instructions
```bash
make
```

## Execution Instructions
```bash
make run
```
or:
```bash
./game
```

## File Structure Overview
```text
.
├── Common.h
├── Event.cpp
├── Event.h
├── Game.cpp
├── Game.h
├── Item.cpp
├── Item.h
├── Makefile
├── Map.cpp
├── Map.h
├── MiniGame.cpp
├── MiniGame.h
├── Player.cpp
├── Player.h
├── README.md
├── SaveSystem.cpp
├── SaveSystem.h
├── data
│   └── save.txt
├── main.cpp
└── maps
    ├── easy_past.txt
    ├── easy_present.txt
    ├── hard_past.txt
    ├── hard_present.txt
    ├── normal_past.txt
    └── normal_present.txt
```

## Gameplay Instructions
1. Choose **New Game**.
2. Select game mode, then difficulty.
3. Use `W/A/S/D` to move and `T` to switch timelines.
4. Collect `K`, activate `S`, and pass opened route to reach `X`.
5. Use `V` to save, `L` to load, `H` for help, `Q` to quit run.

## Notes for Presentation
- The project is modular and suitable for team collaboration.
- Core features are implemented as a playable MVP with clear architecture.
- Three modes demonstrate extensibility while keeping gameplay understandable.
