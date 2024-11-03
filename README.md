# Sorting-SFC Guide

## How to Execute on Linux

1. Create a build directory:
   ```bash
   mkdir build
   ```

2. Move into the build directory:
   ```bash
   cd build
   ```

3. Run CMake to configure the project:
   ```bash
   cmake ..
   ```

4. Compile the project:
   ```bash
   make
   ```

5. Run the program:
   ```bash
   ./src/main (num points) (steps) (alg)
   ```

## Algorithm Options
- **alg = 1**: Fuerza Bruta
- **alg = 2**: Barnes-Hut
- **alg = 3**: DEBUG mode (comparison between algorithms)
