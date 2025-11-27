# JSnake  

![C Language](https://img.shields.io/badge/Language-C-blue.svg)
![Platform-Linux](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows-lightgrey.svg)
![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)
![Terminal Game](https://img.shields.io/badge/Type-Terminal%20Game-orange.svg)
![No Libraries](https://img.shields.io/badge/Dependencies-None-success.svg)

A lightweight, cross-platform **terminal Snake game written in pure C**, built **without ncurses or any external TUI libraries**.  
JSnake renders using ANSI escape codes and handles raw keyboard input on both Linux and Windows.  

---

## Features

- Fully terminal-rendered Snake game (ANSI escape codes)
- Double-buffered screen updates (no flicker)
- Cross-platform input  
  - **Windows:** `conio.h`  
  - **Linux:** `termios`, `select()`
- Bonus ball system (temporary invincibility)
- Dynamic terminal size detection
- Difficulty increases with score
- Clean exit + automatic terminal state restore

---

## Controls

| Key | Action |
|-----|--------|
| **W** | Up |
| **A** | Left |
| **S** | Down |
| **D** | Right |
| **Q** | Quit |

---

## 🛠️ Build & Run

### Linux
```sh
gcc jsnake.c -o jsnake
./jsnake
```

### Windows (MinGW / MSVC)
```sh
gcc jsnake.c -o jsnake
./jsnake
```

---

## Custom Terminal Size

You may optionally pass custom `rows` and `cols`:

```sh
./jsnake 25 80
```

Minimum:
- **20 rows**
- **55 columns**

---

## Gameplay Summary

- Normal food → **+50 points**
- Bonus food → **+100 points** and **5-hit invincibility** (_only cross-collision_)
- Snake wraps around edges
- Speed increases as score increases

---

## Project Structure

```sh
📂 JSnake
├── 📁 img
│   └── 🖼️ jsnake.gif
├── 🪪 LICENSE
├── 📖 README.md
└── 📁 src
    └── 📄 jsnake.c
```

---

## Preview

![JSnake](/img/jsnake.gif)

---

## License

**MIT License**

---

## Author

**Jyotismoy Kalita**
