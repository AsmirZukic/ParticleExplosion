# ParticleExplosion

A high-fidelity particle simulation framework built on the **Perun Engine**. This project demonstrates advanced physics simulations including rigid body collisions and atomic molecular dynamics.

![](https://media.giphy.com/media/qADEBWL5hwAKT4y7KN/giphy.gif)

## Dependencies

*   **SDL2**: Window management and input.
*   **Perun**: A custom game engine (expected to be in a sibling directory or installed).
    *   *Note: This project relies on the Perun Engine for rendering and core systems.*
*   **CMake**: Build system.

## Compilation

Ensure you have `SDL2` and `CMake` installed. The project expects the `Perun` engine to be available.

```bash
mkdir build
cd build
cmake ..
make
```

## Running the Simulation

```bash
./ParticleExplosion
```

## Features

### Mode 1: Collision Simulation (Press `1`)
*   Rigid body physics with elastic collisions.
*   **Quadtree Optimization**: Efficiently handles thousands of particles using $O(N \log N)$ spatial partitioning.
*   **Mouse Interaction**: Left-click to repel particles.

### Mode 2: Atomic Simulation (Press `2`)
*   **High-Fidelity Physics**: 
    *   True Inverse-Square Law ($1/r^2$) electrostatic forces.
    *   8x Time Sub-stepping for stability at high energy.
    *   Hysteresis (Binding Energy) for realistic molecular bonds.
*   **Stoichiometric Chemistry**:
    *   Atoms have integer charges (-2, -1, +1, +2).
    *   Compounds form naturally by neutralizing electric fields.
*   **Controls**:
    *   `Space`: Re-roll particle charge rules.
    *   **Left Click**: Scatter / Repel.
    *   **Right Click**: Catalyze / Attract.

## Performance
Optimized using:
*   **Quadtree Spatial Partitioning** (Templated for generic use).
*   **Sub-stepping** for numerical stability WITHOUT compromising physical accuracy.
