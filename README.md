
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
