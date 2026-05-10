# Chess Game — C++ / SFML
# Members
---
- M.Imran (25F-0800)
- M.Bilal (25F-0502)
- Rana Saad (25F-0707)
---

A fully playable, professional chess application built with **C++17** and **SFML 2.5+**.

---

## Architecture Overview

```
ChessGame/
├── main.cpp                  Entry point
├── CMakeLists.txt            CMake build script
├── include/                  Header files
│   ├── Piece.h               Abstract base class (Abstraction + Polymorphism)
│   ├── Pawn.h / Rook.h / ... Concrete piece classes (Inheritance)
│   ├── Board.h               Game board + rule engine (Composition)
│   ├── Move.h                Move data structure
│   ├── Player.h              Player entity
│   ├── GUIManager.h          SFML rendering layer
│   └── ChessGame.h           Top-level game controller
├── src/                      Implementation files
│   └── *.cpp
└── assets/                   PNG piece images + font
    └── generate_pieces.py    Auto-generates piece images
```

### OOP Design

| Principle       | Where Applied |
|-----------------|---------------|
| Abstraction     | `Piece` is abstract — `getValidMoves()`, `draw()`, `getSymbol()` are pure virtual |
| Inheritance     | `Pawn`, `Rook`, `Knight`, `Bishop`, `Queen`, `King` all inherit `Piece` |
| Polymorphism    | `Board` stores `unique_ptr<Piece>` — virtual dispatch for moves and drawing |
| Encapsulation   | All member data is `private`/`protected`; accessed via getters/setters |
| Composition     | `ChessGame` owns `Board`, `Player`, `GUIManager` |

---

## Features

- Full legal move validation (including check filtering)
- Check, checkmate, and stalemate detection
- **Castling** (kingside and queenside)
- **En-passant** capture
- **Pawn promotion** (dialog to choose piece)
- Last-move highlighting (yellow)
- Selected piece + legal move highlighting (green/blue dots)
- Check highlight (red king square)
- Captured pieces display
- Algebraic notation move history
- Restart (button or `R` key) / Exit (button or `Esc` key)

---

## Build Instructions

### Prerequisites

- CMake 3.16+
- C++17 compiler (MSVC 2019+, GCC 9+, Clang 10+)
- SFML 2.5+ (graphics, window, system components)

---

The CMake build script automatically copies `assets/` next to the executable.

---

## Controls

| Action              | Input                        |
|---------------------|------------------------------|
| Select piece        | Left-click on your piece     |
| Move piece          | Left-click on highlighted square |
| Deselect            | Left-click same piece again  |
| Restart game        | Click **New Game** or press `R` |
| Exit                | Click **Exit** or press `Esc` |

---

## ASCII Board Mockup

```
  ┌─────────────────────────────────────────┐  ┌──────────────────────┐
8 │ r  n  b  q  k  b  n  r │                │  │ CHESS                │
7 │ p  p  p  p  p  p  p  p │                │  │ ┌──────────────────┐ │
6 │ .  .  .  .  .  .  .  . │                │  │ │  Black's Turn    │ │
5 │ .  .  .  .  .  .  .  . │                │  │ └──────────────────┘ │
4 │ .  .  .  .  P  .  .  . │  ← selected    │  │ Captured by White:   │
3 │ .  .  .  .  ●  .  .  . │  ← legal move  │  │ p p                  │
2 │ P  P  P  P  .  P  P  P │                │  │ Move History:        │
1 │ R  N  B  Q  K  B  N  R │                │  │ 1. e4  e5            │
  └─────────────────────────────────────────┘  │ 2. Nf3 Nc6           │
    a  b  c  d  e  f  g  h                     │ [New Game] [Exit]    │
                                               └──────────────────────┘
```
