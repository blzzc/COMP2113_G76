
# Echoes of Time

**Echoes of Time** is a terminal-based puzzle adventure game written in standard C++. The player is trapped inside a ruined structure that exists in two linked timelines: the **Past** and the **Present**.



## Installation&Run Guide

### Run the game on Windows (using WSL) or Linux/macOS
1.Install Required Build Tools
```bash
sudo apt update
sudo apt install build-essential
``
```
2.Enter the Project Directory
```bash
cd "/document location"
```
3.Clean old files
```bash
rm -f game *.o
```
4.Compile the game
```bash
make
```
If you see:
```bash
make: Nothing to be done for 'all'.
```
it means the game is already compiled successfully.
5.Run the game
```bash
./game
```

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
## Screenshots

<img width="1582" height="676" alt="Echoes of Time" src="https://github.com/user-attachments/assets/e08748b8-8246-48d2-b6d7-f0dee372f714" />



## Authors

- [@OLDHUNTER-111](https://github.com/jackshisupreme-lgtm)
See ChunLui 3036620636
- [@Blzzc](https://www.github.com/blzzc)
Zeng Jingda 3036519473
- [@xumuchixu-star](https://github.com/xumuchixu-star) Xu Muchi 3036484032

## Project Structure
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
## Gameplay Instructions
1. Choose **New Game**.
2. Select game mode, then difficulty.
3. Use `W/A/S/D` to move and `T` to switch timelines.
4. Collect `K`, activate `S`, and pass opened route to reach `X`.
5. Use `V` to save, `L` to load, `H` for help, `Q` to quit run.
