# cpp-obj-viewer – Tiny CPU OBJ Model Viewer

This project is a small **C++17 OBJ viewer** with a **CPU software rasterizer**, a **z-buffer**, **simple lighting** and a **SFML HUD**.

It’s designed for **low-poly `.obj` + `.mtl` models** and to be **easy to read** as a portfolio project.

---

## 1. Quick demo

_(GIFs coming soon – example placeholders)_

- ![Tree model](assets/gifs/tree.gif)
- ![Fox model](assets/gifs/fox.gif)

The viewer:
- loads a low-poly model
- applies per-face diffuse lighting
- lets you toggle **wireframe** and **auto-rotation** from the HUD.

---

## 2. Features

- **OBJ loader**
  - vertices (`v`), faces (`f`), optional normals (`vn`)
  - faces treated as triangles

- **Basic MTL materials**
  - loads materials by name (`newmtl`)
  - uses diffuse color (`Kd r g b`) as base color
  - falls back to **white** if no material / mismatch

- **CPU software rasterizer**
  - manual projection + triangle rasterization
  - depth handled by a `std::vector<float>` z-buffer
  - correct visibility: nearer triangles overwrite farther ones

- **Simple lighting**
  - one directional light
  - per-face normal, Lambert shading: `max(0, dot(n, lightDir))`

- **Wireframe mode**
  - draws only triangle edges
  - handy to debug / show mesh topology

- **Auto-rotation**
  - model rotates around Y axis
  - good for demos and GIFs

- **SFML HUD**
  - window + events
  - text info (model / material)
  - buttons for:
    - **Wireframe** on/off  
    - **Auto-rotate** on/off  

---

## 3. Build

Requirements:

- **C++17**
- **SFML 3** (Graphics, Window, System)

### macOS (Homebrew)

```bash
brew install sfml
make
./viewer
```

If SFML is in a custom path, adjust the `Makefile`, e.g.:

```make
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -Iinclude -I/opt/homebrew/opt/sfml/include
LDFLAGS  = -L/opt/homebrew/opt/sfml/lib -lsfml-graphics -lsfml-window -lsfml-system
```

### Linux (example)

```bash
sudo apt install libsfml-dev
make
./viewer
```

---

## 4. Usage

The viewer expects:

```bash
./viewer <model.obj> [material.mtl]
```

Examples (from repo root):

```bash
# Tree
./viewer assets/models/tree/tree-branched.obj assets/models/tree/tree-branched.mtl

# Fox
./viewer assets/models/fox/RedFox.obj assets/models/fox/RedFox.mtl

# Whale
./viewer assets/models/whale/Whale.obj assets/models/whale/Whale.mtl
```

If you omit the MTL file:

```bash
./viewer assets/models/tree/tree-branched.obj
```

the model is still rendered, with **white** faces + lighting.

---

## 5. Controls

**Keyboard**

- `←` / `→` – rotate model around Y axis  
- `↑` / `↓` – rotate model around X axis  
- `W` – zoom in  
- `S` – zoom out  

**Mouse / HUD**

- click **Wireframe** – toggle edge-only rendering  
- click **Auto-rotate** – toggle automatic rotation  

---

## 6. Adding your own models

1. Put your files under `assets/models/...`, e.g.:

   ```text
   assets/models/dragon/
     dragon.obj
     dragon.mtl
   ```

2. In the OBJ:
   - reference the material file: `mtllib dragon.mtl`
   - use material names: `usemtl dragon_body`

3. In the MTL:
   - define materials with `newmtl <name>`
   - set diffuse color with `Kd r g b` (values in `[0.0, 1.0]`)

4. Run:

   ```bash
   ./viewer assets/models/dragon/dragon.obj assets/models/dragon/dragon.mtl
   ```

---

## 7. Project structure

```text
.
├── include/
│   ├── App.hpp        # Main loop, events, HUD
│   ├── Renderer.hpp   # CPU rasterizer + z-buffer + lighting
│   ├── Model.hpp      # OBJ/MTL loading and storage
│   └── Math.hpp       # Small math helpers (vec, mat, etc.)
├── src/
│   ├── main.cpp
│   ├── App.cpp
│   ├── Renderer.cpp
│   ├── Model.cpp
│   └── Math.cpp
├── assets/
│   └── models/
│       ├── tree/
│       ├── fox/
│       └── whale/
├── Makefile
└── README.md
```

The goal is to keep the code **small, explicit and easy to read** as a learning / portfolio project.
