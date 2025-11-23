# cpp-obj-viewer

A small **C++17** OBJ model viewer with a **CPU z-buffer**, **simple lighting** and a **SFML HUD**.  
Designed for **low-poly models** (`.obj` + `.mtl`) and to be easy to read as a portfolio project.

---

## ✨ Features (with explanations)

- **OBJ geometry loading**  
  The viewer parses standard `.obj` files:
  - vertex positions (`v`)
  - face indices (`f`)
  - optional vertex normals (`vn`)  
  Faces are treated as triangles and stored in a simple internal structure for rendering.

- **MTL material support (basic)**  
  If you provide a `.mtl` file and `usemtl` entries in the `.obj`, the viewer:
  - loads materials by name (`newmtl`)
  - reads diffuse color (`Kd r g b`)
  - associates each face with a material  
  If no material is found, the model is rendered in plain white.

- **Software rasterizer with CPU z-buffer**  
  The rendering pipeline is done on the CPU:
  - each triangle is transformed and projected to screen space
  - a depth buffer (z-buffer) is stored as a `std::vector<float>`
  - pixels are drawn only if they are closer than the previously stored depth  
  This allows correct visibility (far triangles do not overwrite nearer ones).

- **Simple diffuse lighting (per-face)**  
  For each triangle:
  - a face normal is computed in world/view space
  - a directional light is applied (Lambert: `intensity = max(0, dot(normal, lightDir))`)
  - the base material color is multiplied by this intensity  
  Result: faces directly facing the light are bright, others are darker.

- **Wireframe mode**  
  A dedicated rendering mode draws only the edges of triangles:
  - useful to debug geometry
  - visually shows the mesh topology  
  This mode is toggled via a HUD button.

- **Auto-rotation**  
  The model can rotate automatically around the Y axis:
  - allows you to showcase a model hands-free
  - good for demos and GIF recordings  
  Auto-rotate is toggled via a HUD button.

- **Keyboard controls for camera / model**  
  - **Arrow keys**: rotate model around X and Y axes  
  - **W / S**: zoom in / zoom out (change scale)  
  This makes it easy to inspect the model from any angle.

- **HUD overlay with SFML**  
  The project uses SFML to:
  - open a window and handle events
  - draw text information (current model, material name)
  - draw simple UI buttons for:
    - wireframe on/off
    - auto-rotate on/off  

- **Clean architecture (for reading the code)**  
  The code is split into small, focused classes:
  - `Model` – loads and stores OBJ/MTL data
  - `Renderer` – CPU rasterizer, z-buffer, lighting
  - `App` – window loop, input handling, HUD  
  The goal is to keep each file and function easy to understand.

---

## 🎮 Controls

**Keyboard**

- `←` / `→` : rotate model around Y axis  
- `↑` / `↓` : rotate model around X axis  
- `W` : zoom in  
- `S` : zoom out  

**Mouse**

- Click on the **HUD buttons** at the bottom of the window:
  - **Wireframe**: toggle edge-only rendering
  - **Auto-rotate**: toggle continuous rotation

---

## 📁 Project structure

Example structure (you can adapt to your repo):

```text
cpp-obj-viewer/
  src/
    main.cpp
    App.cpp
    Renderer.cpp
    Model.cpp
    Math.cpp
    ...
  include/
    App.hpp
    Renderer.hpp
    Model.hpp
    Math.hpp
    ...
  assets/
    models/
      tree/
        tree-branched.obj
        tree-branched.mtl
      fox/
        RedFox.obj
        RedFox.mtl
      whale/
        Whale.obj
        Whale.mtl
  Makefile
  README.md
```

- `src/` : implementation files  
- `include/` : headers for the main classes  
- `assets/models/` : example low-poly models (tree, fox, whale, etc)  
- `Makefile` : build rules (g++, C++17, SFML)

---

## 🔧 Building

The project uses:

- **C++17**
- **SFML 3** (Graphics + Window + System)

### Example (macOS with Homebrew)

Install SFML 3 (if not already):

```bash
brew install sfml
```

Build:

```bash
make
```

This will produce an executable named:

```bash
./viewer
```

If your SFML headers or libraries are not in the default locations, you may need to adjust include / lib paths in the `Makefile`, for example:

```make
CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -Iinclude -I/opt/homebrew/opt/sfml/include
LDFLAGS = -L/opt/homebrew/opt/sfml/lib           -lsfml-graphics -lsfml-window -lsfml-system
```

### Linux (example)

Install SFML (package name may vary by distro):

```bash
sudo apt install libsfml-dev
```

Then:

```bash
make
```

---

## ▶️ Running

The viewer expects:

- an **OBJ file** path
- optionally: a **MTL file** path

Examples assuming you are in the repo root:

```bash
# Tree
./viewer assets/models/tree/tree-branched.obj assets/models/tree/tree-branched.mtl

# Fox
./viewer assets/models/fox/RedFox.obj assets/models/fox/RedFox.mtl

# Whale
./viewer assets/models/whale/Whale.obj assets/models/whale/Whale.mtl
```

If you **omit the MTL file**:

```bash
./viewer assets/models/tree/tree-branched.obj
```

- The model still loads
- Faces are rendered in **white** with lighting (default color)

---

## 🎨 Materials & Colors

The viewer supports a minimal subset of the **MTL format**:

- `newmtl <name>`  
  Starts a new material with the given name.

- `Kd r g b`  
  Diffuse color used as the base color of the face.  
  Each `r g b` is expected in the range `[0.0, 1.0]`.

In your `.obj`, you must reference materials like this:

```obj
mtllib tree-branched.mtl

usemtl tree_bark_dark
f 1 2 3
f 2 3 4

usemtl tree_green
f 5 6 7
f 6 7 8
```

If:

- the `.mtl` file is missing  
- or the `usemtl` name doesn’t match any loaded material  

→ the viewer falls back to **white** for that face.

---

## 🧠 How it works (high-level pipeline)

1. **Load model**
   - `Model` parses OBJ vertices, faces and (optionally) normals
   - If a MTL file is provided, it loads materials and assigns them to faces

2. **Prepare camera & transforms**
   - The model is centered and scaled
   - Rotation around X/Y is updated from keyboard input or auto-rotate

3. **Transform to view space**
   - Each vertex goes from model space → view space using rotation + scale

4. **Lighting**
   - A directional light is defined (e.g. from above / front)
   - For each face, a normal is computed and used for diffuse lighting

5. **Projection**
   - Vertices are projected to 2D screen coordinates (simple perspective or orthographic style)
   - Coordinates are remapped to the window size

6. **Rasterization with z-buffer**
   - For each triangle, a bounding box is computed
   - For every pixel in this box:
     - barycentric coordinates are used to interpolate depth
     - if this depth is closer than the stored depth → pixel is drawn and depth updated

7. **Display**
   - The CPU-rendered image is copied into a SFML texture
   - The texture is drawn as a full-screen sprite
   - Text + HUD buttons are drawn on top

---

## ➕ Adding your own models

1. **Put your files in `assets/models/...`**, e.g.:

   ```text
   assets/models/dragon/
     dragon.obj
     dragon.mtl
   ```

2. **Make sure:**
   - the `.obj` contains a line `mtllib dragon.mtl`
   - the `.mtl` contains `newmtl <name>` and `Kd r g b` lines
   - the `.obj` uses `usemtl <name>` for different parts

3. **Run:**

   ```bash
   ./viewer assets/models/dragon/dragon.obj assets/models/dragon/dragon.mtl
   ```

---
